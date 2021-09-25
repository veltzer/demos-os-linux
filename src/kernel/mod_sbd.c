// SPDX-License-Identifier: GPL-2.0
/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-linux. If not, see <http://www.gnu.org/licenses/>.
 */

/* #define DEBUG */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/blk_types.h>
#include <linux/blkdev.h>
#include <linux/blk-mq.h>
#include <uapi/linux/hdreg.h> //for struct hd_geometry
#include <uapi/linux/cdrom.h> //for CDROM_GET_CAPABILITY

/*
 * Check this out:
 * https://prog.world/linux-kernel-5-0-we-write-simple-block-device-under-blk-mq/
 * https://github.com/CodeImp/sblkdev
 */

#ifndef SUCCESS
#define SUCCESS 0
#endif

//This defines are available in blkdev.h from kernel 4.17 (vanilla).
#ifndef SECTOR_SHIFT
#define SECTOR_SHIFT 9
#endif
#ifndef SECTOR_SIZE
#define SECTOR_SIZE (1 << SECTOR_SHIFT)
#endif

// constants - instead defines
static const char *_sblkdev_name = "sblkdev";
static const size_t _sblkdev_buffer_size = 16 * PAGE_SIZE;

// types
struct sblkdev_cmd_s {
	//nothing
};

// The internal representation of our device
struct sblkdev_device_s {
	sector_t capacity;				// Device size in bytes
	u8 *data;				// The data aray. u8 - 8 bytes
	atomic_t open_counter;			// How many openers

	struct blk_mq_tag_set tag_set;
	struct request_queue *queue;	// For mutual exclusion

	struct gendisk *disk;		// The gendisk structure
};

// global variables

static int _sblkdev_major;
static struct sblkdev_device_s *_sblkdev_device;


// functions
static int sblkdev_allocate_buffer(struct sblkdev_device_s *dev)
{
	dev->capacity = _sblkdev_buffer_size >> SECTOR_SHIFT;
	dev->data = kmalloc(dev->capacity << SECTOR_SHIFT, GFP_KERNEL); //
	if (dev->data == NULL) {
		pr_warn("sblkdev: vmalloc failure.\n");
		return -ENOMEM;
	}

	return SUCCESS;
}

static void sblkdev_free_buffer(struct sblkdev_device_s *dev)
{
	// kfree(NULL) is safe
	kfree(dev->data);
	dev->data = NULL;
	dev->capacity = 0;
}

static void sblkdev_remove_device(void)
{
	struct sblkdev_device_s *dev = _sblkdev_device;

	if (dev == NULL)
		return;

	if (dev->disk)
		del_gendisk(dev->disk);

	if (dev->queue) {
		blk_cleanup_queue(dev->queue);
		dev->queue = NULL;
	}

	if (dev->tag_set.tags)
		blk_mq_free_tag_set(&dev->tag_set);

	if (dev->disk) {
		put_disk(dev->disk);
		dev->disk = NULL;
	}

	sblkdev_free_buffer(dev);

	kfree(dev);
	_sblkdev_device = NULL;

	pr_warn("sblkdev: simple block device was removed\n");
}

static int do_simple_request(struct request *rq, unsigned int *nr_bytes)
{
	int ret = SUCCESS;
	struct bio_vec bvec;
	struct req_iterator iter;
	struct sblkdev_device_s *dev = rq->q->queuedata;
	loff_t pos = blk_rq_pos(rq) << SECTOR_SHIFT;
	loff_t dev_size = (loff_t)(dev->capacity << SECTOR_SHIFT);

	pr_warn("sblkdev: request start from sector %lld\n", blk_rq_pos(rq));

	rq_for_each_segment(bvec, rq, iter) {
		unsigned long b_len = bvec.bv_len;
		void *b_buf = page_address(bvec.bv_page) + bvec.bv_offset;

		if ((pos + b_len) > dev_size)
			b_len = (unsigned long)(dev_size - pos);
		if (rq_data_dir(rq))//WRITE
			memcpy(dev->data + pos, b_buf, b_len);
		else//READ
			memcpy(b_buf, dev->data + pos, b_len);

		pos += b_len;
		*nr_bytes += b_len;
	}

	return ret;
}

static blk_status_t _queue_rq(struct blk_mq_hw_ctx *hctx, const struct blk_mq_queue_data *bd)
{
	unsigned int nr_bytes = 0;
	blk_status_t status = BLK_STS_OK;
	struct request *rq = bd->rq;

	//we cannot use any locks that make the thread sleep
	blk_mq_start_request(rq);

	if (do_simple_request(rq, &nr_bytes) != SUCCESS)
		status = BLK_STS_IOERR;

	pr_warn("sblkdev: request process %d bytes\n", nr_bytes);

	if (blk_update_request(rq, status, nr_bytes)) //GPL-only symbol
		BUG();
	__blk_mq_end_request(rq, status);
	return BLK_STS_OK;//always return ok
}

static struct blk_mq_ops _mq_ops = {
	.queue_rq = _queue_rq,
};

static int _open(struct block_device *bdev, fmode_t mode)
{
	struct sblkdev_device_s *dev = bdev->bd_disk->private_data;

	if (dev == NULL) {
		pr_warn("sblkdev: invalid disk private_data\n");
		return -ENXIO;
	}

	atomic_inc(&dev->open_counter);

	pr_warn("sblkdev: device was opened\n");

	return SUCCESS;
}
static void _release(struct gendisk *disk, fmode_t mode)
{
	struct sblkdev_device_s *dev = disk->private_data;

	if (dev) {
		atomic_dec(&dev->open_counter);
		pr_warn("sblkdev: device was closed\n");
	} else
		pr_warn("sblkdev: invalid disk private_data\n");
}

static int _getgeo(struct sblkdev_device_s *dev, struct hd_geometry *geo)
{
	sector_t quotient;

	geo->start = 0;
	if (dev->capacity > 63) {
		geo->sectors = 63;
		quotient = (dev->capacity + (63 - 1)) / 63;

		if (quotient > 255) {
			geo->heads = 255;
			geo->cylinders = (unsigned short)((quotient + (255 - 1)) / 255);
		} else {
			geo->heads = (unsigned char)quotient;
			geo->cylinders = 1;
		}
	} else {
		geo->sectors = (unsigned char)dev->capacity;
		geo->cylinders = 1;
		geo->heads = 1;
	}
	return SUCCESS;
}

static int _ioctl(struct block_device *bdev, fmode_t mode, unsigned int cmd, unsigned long arg)
{
	int ret = -ENOTTY;
	struct sblkdev_device_s *dev = bdev->bd_disk->private_data;

	pr_warn("sblkdev: ioctl %x received\n", cmd);

	switch (cmd) {
	case HDIO_GETGEO:
	{
	struct hd_geometry geo;

		ret = _getgeo(dev, &geo);
	if (copy_to_user((void *)arg, &geo, sizeof(struct hd_geometry)))
		ret = -EFAULT;
	else
		ret = SUCCESS;
	break;
	}
	case CDROM_GET_CAPABILITY: //0x5331  / * get capabilities * /
	{
	struct gendisk *disk = bdev->bd_disk;

	if (bdev->bd_disk && (disk->flags & GENHD_FL_CD))
		ret = SUCCESS;
	else
		ret = -EINVAL;
	break;
	}
	}

	return ret;
}
#ifdef CONFIG_COMPAT
static int _compat_ioctl(struct block_device *bdev, fmode_t mode, unsigned int cmd, unsigned long arg)
{
	// CONFIG_COMPAT is to allow running 32-bit userspace code on a 64-bit kernel
	return -ENOTTY; // not supported
}
#endif

static const struct block_device_operations _fops = {
	.owner = THIS_MODULE,
	.open = _open,
	.release = _release,
	.ioctl = _ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = _compat_ioctl,
#endif
};

//
static int sblkdev_add_device(void)
{
	int ret = SUCCESS;

	struct sblkdev_device_s *dev = kzalloc(sizeof(struct sblkdev_device_s), GFP_KERNEL);

	/* this is not needed (out-of-memory is unlikely...)
	 * if (dev == NULL) {
	 *	pr_warn("sblkdev: unable to allocate %ld bytes\n", sizeof(struct sblkdev_device_s));
	 *	return -ENOMEM;
	 * }
	 */
	_sblkdev_device = dev;

	do {
		ret = sblkdev_allocate_buffer(dev);
		if (ret)
			break;

	{	//configure tag_set
		dev->tag_set.ops = &_mq_ops;
		dev->tag_set.nr_hw_queues = 1;
		dev->tag_set.queue_depth = 128;
		dev->tag_set.numa_node = NUMA_NO_NODE;
		dev->tag_set.cmd_size = sizeof(struct sblkdev_cmd_s);
		dev->tag_set.flags = BLK_MQ_F_SHOULD_MERGE;
		dev->tag_set.driver_data = dev;

		ret = blk_mq_alloc_tag_set(&dev->tag_set);
	if (ret) {
		pr_warn("sblkdev: unable to allocate tag set\n");
		break;
	}
	}

	{//configure queue
	struct request_queue *queue = blk_mq_init_queue(&dev->tag_set);

	if (IS_ERR(queue)) {
		ret = PTR_ERR(queue);
		pr_warn("sblkdev: Failed to allocate queue\n");
		break;
	}
		dev->queue = queue;
	}
	dev->queue->queuedata = dev;

	{// configure disk
	struct gendisk *disk = alloc_disk(1); //only one partition

	if (disk == NULL) {
		pr_warn("sblkdev: Failed to allocate disk\n");
		ret = -ENOMEM;
		break;
	}

		disk->flags |= GENHD_FL_NO_PART_SCAN; //only one partition
		//disk->flags |= GENHD_FL_EXT_DEVT;
		disk->flags |= GENHD_FL_REMOVABLE;

		disk->major = _sblkdev_major;
		disk->first_minor = 0;
		disk->fops = &_fops;
		disk->private_data = dev;
		disk->queue = dev->queue;
		sprintf(disk->disk_name, "sblkdev%d", 0);
		set_capacity(disk, dev->capacity);

		dev->disk = disk;
		add_disk(disk);
	}

		pr_warn("sblkdev: simple block device was created\n");
	} while (false);

	if (ret) {
		sblkdev_remove_device();
		pr_warn("sblkdev: Failed add block device\n");
	}

	return ret;
}

static int __init sblkdev_init(void)
{
	int ret = SUCCESS;

	_sblkdev_major = register_blkdev(_sblkdev_major, _sblkdev_name);
	if (_sblkdev_major <= 0) {
		pr_warn("sblkdev: unable to get major number\n");
		return -EBUSY;
	}

	ret = sblkdev_add_device();
	if (ret)
		unregister_blkdev(_sblkdev_major, _sblkdev_name);

	return ret;
}

static void __exit sblkdev_exit(void)
{
	sblkdev_remove_device();

	if (_sblkdev_major > 0)
		unregister_blkdev(_sblkdev_major, _sblkdev_name);
}

module_init(sblkdev_init);
module_exit(sblkdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Code Imp");
