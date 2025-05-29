// SPDX-License-Identifier: GPL-2.0
/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/vmalloc.h>
#include <linux/blkdev.h>
#include <linux/blk-mq.h>
#include <linux/hdreg.h>
#include <linux/version.h>

/*
 * FLAGS: NOCHECKPATCH
 */

#define MEMBLOCK_MAJOR 240
#define MEMBLOCK_SIZE (1024 * 1024) // 1MB

static int memblock_major = MEMBLOCK_MAJOR;
static int nsectors = MEMBLOCK_SIZE / 512; // Using 512 as the sector size

struct memblock_dev {
	int size;
	u8 *data;
	struct blk_mq_tag_set tag_set;
	struct gendisk *gd;
};

// initialized automatically to NULL
static struct memblock_dev *memblock_device;

static int memblock_open(struct gendisk *disk, blk_mode_t mode)
{
	return 0;
}

static void memblock_release(struct gendisk *disk)
{
}

static int memblock_getgeo(struct block_device *bdev, struct hd_geometry *geo)
{
	geo->heads = 1;
	geo->cylinders = 32;
	geo->sectors = 32;
	geo->start = 0;
	return 0;
}

static const struct block_device_operations memblock_ops = {
	.owner = THIS_MODULE,
	.open = memblock_open,
	.release = memblock_release,
	.getgeo = memblock_getgeo
};

static void memblock_transfer(struct memblock_dev *dev, sector_t sector,
							unsigned int nsect, char *buffer, int write)
{
	unsigned long offset = sector * 512;
	unsigned long nbytes = nsect * 512;

	if ((offset + nbytes) > dev->size) {
		pr_notice("memblock: Beyond-end %s (%lu %u)\n",
				(write ? "write" : "read"), offset, nsect);
		return;
	}

	if (write)
		memcpy(dev->data + offset, buffer, nbytes);
	else
		memcpy(buffer, dev->data + offset, nbytes);
}

static blk_status_t memblock_request(struct blk_mq_hw_ctx *hctx,
									 const struct blk_mq_queue_data *bd)
{
	struct request *req = bd->rq;
	struct memblock_dev *dev = req->q->queuedata;
	struct bio_vec bvec;
	struct req_iterator iter;
	sector_t sector = blk_rq_pos(req);
	int write = rq_data_dir(req) == WRITE;

	blk_mq_start_request(req);

	rq_for_each_segment(bvec, req, iter) {
		char *buffer = page_address(bvec.bv_page) + bvec.bv_offset;
		unsigned int len = bvec.bv_len;

		memblock_transfer(dev, sector, len / 512, buffer, write);
		sector += len / 512;
	}

	blk_mq_end_request(req, BLK_STS_OK);
	return BLK_STS_OK;
}

static struct blk_mq_ops memblock_mq_ops = {
	.queue_rq = memblock_request,
};

static int __init memblock_init(void)
{
	memblock_device = kzalloc(sizeof(struct memblock_dev), GFP_KERNEL);
	if (!memblock_device)
		return -ENOMEM;

	memblock_device->size = MEMBLOCK_SIZE;

	memblock_device->data = vmalloc(memblock_device->size);
	if (!memblock_device->data) {
		kfree(memblock_device);
		return -ENOMEM;
	}

	memblock_major = register_blkdev(memblock_major, "memblock");
	if (memblock_major <= 0) {
		pr_warn("memblock: unable to get major number\n");
		vfree(memblock_device->data);
		kfree(memblock_device);
		return -EBUSY;
	}

	int err = blk_mq_alloc_sq_tag_set(&memblock_device->tag_set, &memblock_mq_ops,
									64, BLK_MQ_F_SHOULD_MERGE);
	if (err) {
		unregister_blkdev(memblock_major, "memblock");
		vfree(memblock_device->data);
		kfree(memblock_device);
		return err;
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 11, 0)
	memblock_device->gd = blk_mq_alloc_disk(&memblock_device->tag_set, NULL, memblock_device);
#else
	memblock_device->gd = blk_mq_alloc_disk(&memblock_device->tag_set, NULL);
#endif
	if (IS_ERR(memblock_device->gd)) {
		blk_mq_free_tag_set(&memblock_device->tag_set);
		unregister_blkdev(memblock_major, "memblock");
		vfree(memblock_device->data);
		kfree(memblock_device);
		return PTR_ERR(memblock_device->gd);
	}

	memblock_device->gd->major = memblock_major;
	memblock_device->gd->first_minor = 0;
	memblock_device->gd->minors = 1;
	memblock_device->gd->fops = &memblock_ops;
	memblock_device->gd->private_data = memblock_device;
	snprintf(memblock_device->gd->disk_name, DISK_NAME_LEN, "memblock");
	set_capacity(memblock_device->gd, nsectors);

	err = add_disk(memblock_device->gd);
	if (err) {
		put_disk(memblock_device->gd);
		blk_mq_free_tag_set(&memblock_device->tag_set);
		unregister_blkdev(memblock_major, "memblock");
		vfree(memblock_device->data);
		kfree(memblock_device);
		return err;
	}

	return 0;
}

static void __exit memblock_exit(void)
{
	if (memblock_device) {
		del_gendisk(memblock_device->gd);
		put_disk(memblock_device->gd);
		blk_mq_free_tag_set(&memblock_device->tag_set);
		unregister_blkdev(memblock_major, "memblock");
		vfree(memblock_device->data);
		kfree(memblock_device);
	}
}

module_init(memblock_init);
module_exit(memblock_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple ramdisk block driver");
