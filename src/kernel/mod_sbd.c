/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2017 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

/* #define DEBUG */
#include <linux/module.h> /* for the MODULE_* stuff */
#include <linux/hdreg.h> /* for geometry of hard drive */
#include <linux/blkdev.h> /* for block operations */
#include <linux/vmalloc.h> /* for vmalloc */
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("A simple demo of how to write a block device driver");
MODULE_VERSION("1.4.0");

/*
* A sample, extra-simple block driver. Updated for kernel 2.6.31.
*
* (C) 2003 Eklektix, Inc.
* (C) 2010 Pat Patterson <pat at superpat dot com>
* Redistributable under the terms of the GNU GPL.
* Changed by Mark Veltzer to match newer kernels, added comments and debug.
*
* References:
* LDD3 - chapter on block device drivers.
* http://lwn.net/Articles/58720/
* http://blog.superpat.com/2010/05/04/
*	a-simple-block-driver-for-linux-kernel-2-6-31/
*
* TODO:
* - add a user space script that shows how to format, mount use and unmount
* the device.
*/

static int major_num;
module_param(major_num, int, 0);
static int logical_block_size = 512;
module_param(logical_block_size, int, 0);
/* How big the drive is, this is number of sectors where each sector is
 * as above */
static int nsectors = 65536;
module_param(nsectors, int, 0);
static int debug;
static int dowork = 1;

/*
* We can tweak our hardware sector size, but the kernel talks to us
* in terms of small sectors, always.
*/
static const int KERNEL_SECTOR_SIZE = 512;

/*
* Our request queue.
*/
static struct request_queue *Queue;

/*
* The internal representation of our device.
*/
static struct sbd_device {
	unsigned long size;
	spinlock_t lock;
	u8 *data;
	struct gendisk *gd;
} Device;

/*
* This is our own helper function to handle a single read or write request.
* We obviously do a memcpy since our device is very simple.
*/
static void sbd_transfer(struct sbd_device *dev, sector_t sector,
		unsigned long nsect, char *buffer, int write)
{
	unsigned long offset = sector * logical_block_size;
	unsigned long nbytes = nsect * logical_block_size;

	if ((offset + nbytes) > dev->size) {
		pr_notice("sbd: Beyond-end write (%ld %ld)\n", offset, nbytes);
		return;
	}
	if (write)
		memcpy(dev->data + offset, buffer, nbytes);
	else
		memcpy(buffer, dev->data + offset, nbytes);
}

/*
* Handle many I/O requests on the queue. This is a contract function
* with the kernel. This is the function that will be called by the kernel
* for us to handle requests.
* We pump the request queue here asking for requests. Each request is a
* structure with all the data of the request (is it read or write, where
* from, how much,...).
*/

/* this is needed for newer kernels (2.6.38) that don't have
 * 'blk_fs_request'... */
// #define blk_fs_request(rq) ((rq)->cmd_type == REQ_TYPE_FS)

static void sbd_request(struct request_queue *q)
{
	struct request *req;
	req = blk_fetch_request(q);
	while (req != NULL) {
		if (!blk_fs_request(req)) {
			/* we go a request that we can not handle.
			* We give errors on these.
			* a real device should handle these as well... */
			pr_notice("Skip non-CMD request\n");
			__blk_end_request_all(req, -EIO);
			continue;
		}
		/* from now on we know that we have a read or write request */
		if (debug) {
			PR_DEBUG(
		"sectors is %u, pos is %llu, buffer is %p, rq_data_dir is %d",
				blk_rq_sectors(req),
				blk_rq_pos(req),
				bio_data(req->bio),
				/* req->buffer, */
				rq_data_dir(req)
			);
		}
		if (dowork) {
			sbd_transfer(&Device, blk_rq_pos(req),
					blk_rq_cur_sectors(req),
					bio_data(req->bio),
					/* req->buffer, */
					rq_data_dir(req));
			if (!__blk_end_request_cur(req, 0))
				req = blk_fetch_request(q);
		}
	}
}

/*
* The HDIO_GETGEO ioctl is handled in blkdev_ioctl(), which
* calls this. We need to implement getgeo, since we can't
* use tools such as fdisk to partition the drive otherwise.
*/
int sbd_getgeo(struct block_device *block_device, struct hd_geometry *geo)
{
	long size;

	/* We have no real geometry, of course, so make something up. */
	size = Device.size * (logical_block_size / KERNEL_SECTOR_SIZE);
	geo->cylinders = (size & ~0x3f) >> 6;
	geo->heads = 4;
	geo->sectors = 16;
	geo->start = 0;
	return 0;
}

/*
* The device operations structure.
*/
static const struct block_device_operations sbd_ops = {
	.owner = THIS_MODULE,
	.getgeo = sbd_getgeo
};

static int __init sbd_init(void)
{
	/*
	* Set up our internal device.
	*/
	int err;
	Device.size = nsectors * logical_block_size;
	/* setup a spin lock to be used for the device */
	spin_lock_init(&Device.lock);
	/* allocate the device (we use the .data user pointer to store it) */
	Device.data = vmalloc(Device.size);
	if (IS_ERR(Device.data)) {
		err = PTR_ERR(Device.data);
		goto out_err;
	}
	/*
	* Get a request queue.
	*/
	Queue = blk_init_queue(sbd_request, &Device.lock);
	if (IS_ERR(Queue)) {
		err = PTR_ERR(Queue);
		goto out_free;
	}
	blk_queue_logical_block_size(Queue, logical_block_size);
	/*
	* Get registered.
	*/
	major_num = register_blkdev(major_num, "sbd");
	if (major_num <= 0) {
		pr_warn("sbd: unable to get major number\n");
		err = major_num;
		goto out_queue;
	}
	pr_warn("major number is %d\n", major_num);
	/*
	* And the gendisk structure.
	*/
	Device.gd = alloc_disk(16);
	if (IS_ERR(Device.gd)) {
		err = PTR_ERR(Device.gd);
		goto out_unregister;
	}
	Device.gd->major = major_num;
	Device.gd->first_minor = 0;
	Device.gd->fops = &sbd_ops;
	Device.gd->private_data = &Device;
	sprintf(Device.gd->disk_name, "%s%d", THIS_MODULE->name, 0);
	set_capacity(Device.gd, nsectors);
	Device.gd->queue = Queue;
	add_disk(Device.gd);

	return 0;

/* out_deldisk:
	del_gendisk(Device.gd);
	put_disk(Device.gd);
*/
out_unregister:
	unregister_blkdev(major_num, "sbd");
out_queue:
	blk_cleanup_queue(Queue);
out_free:
	vfree(Device.data);
out_err:
	return err;
}

static void __exit sbd_exit(void)
{
	del_gendisk(Device.gd);
	put_disk(Device.gd);
	unregister_blkdev(major_num, "sbd");
	blk_cleanup_queue(Queue);
	vfree(Device.data);
}

module_init(sbd_init);
module_exit(sbd_exit);
