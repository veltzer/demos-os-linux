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

#include <linux/module.h> /* for MODULE_*, module_*, printk */
#include <linux/fs.h> /* for fops */
#include <linux/device.h> /* for class_create */
#include <linux/slab.h> /* for kmalloc */
#include <linux/uaccess.h> /* for copy_to_user, access_ok */
#include <linux/gfp.h> /* for get_aaaed_page */
#include <linux/cdev.h> /* for cdev_* */
#include <linux/sched.h> /* for cond_resched */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("A simple implementation for something like /dev/aaa");

/*
 * This is an implementation of a kernel module which exposes a /dev/aaa like
 * device to user space. Whenever someone reads from the device the buffer passed
 * to kernel gets filled with aaas.
 * If you want to see the real implementation in the kernel see:
 * $KERNEL_SOURCES/drivers/char/mem.c
 */

/* how many minors do we need ? */
const int MINOR_COUNT = 1;
char buf[256] = {'a'};

/* these are the actual operations */

static ssize_t read_aaa(struct file *file, char __user *buf, size_t count,
		loff_t *ppos)
{
	size_t remaining;
	int ret;

	ret = access_ok(buf, count);
	if (!ret)
		return ret;

	remaining = count;
	while (remaining) {
		ret = __put_user('a', buf++);
		if (ret < 0)
			return ret;
		remaining--;
	}
	return count;
#ifdef ALTERNATE
	ret = access_ok(buf, count);
	size_t remaining;

	remaining = count;
	while (remaining) {
		ssize_t current_transfer = min(remining, 128);

		ret = __copy_to_user(buf, mybuf, current_transfer);
		if (ret)
			return ret;
		remaining -= current_transfer;
	}
#endif // ALTERNATE
}

/* this is the operations table */
static const struct file_operations aaa_fops = {
	.owner = THIS_MODULE,
	.read = read_aaa,
};

/* this variable will store the class */
static struct class *my_class;
/* this variable will store the device */
static struct device *my_device;
/* this variable will hold our cdev struct */
static struct cdev my_cdev;
/* this is the first dev_t allocated to us... */
static dev_t first_dev;
/* this is our first minor. automatically initialised to 0... */
static int first_minor;

static int aaa_init(void)
{
	int err = 0;
	/* allocate our own range of devices */
	err = alloc_chrdev_region(&first_dev, first_minor, MINOR_COUNT,
			THIS_MODULE->name);
	if (err) {
		pr_err("cannot alloc_chrdev_region");
		goto err_final;
	}
	pr_info("allocated the region\n");
	/* add the cdev structure, no error codes */
	cdev_init(&my_cdev, &aaa_fops);
	err = cdev_add(&my_cdev, first_dev, MINOR_COUNT);
	if (err) {
		pr_err("cannot cdev_add\n");
		goto err_dealloc;
	}
	pr_info("added the cdev\n");
	/* this is creating a new class (/proc/devices) */
	my_class = class_create(THIS_MODULE->name);
	if (IS_ERR(my_class)) {
		pr_err("failed in class_create\n");
		err = PTR_ERR(my_class);
		goto err_cdev_del;
	}
	pr_info("created the class\n");
	/* and now lets auto-create a /dev/ node */
	my_device = device_create(my_class, NULL, first_dev, NULL, "%s",
			THIS_MODULE->name);
	if (IS_ERR(my_device)) {
		pr_err("failed in device_create\n");
		err = PTR_ERR(my_device);
		goto err_class;
	}
	pr_info("created the device\n");
	pr_info("loaded ok\n");
	return 0;
/* err_device:
 *	device_destroy(my_class, first_dev);
 */
err_class:
	class_destroy(my_class);
err_cdev_del:
	cdev_del(&my_cdev);
err_dealloc:
	unregister_chrdev_region(first_dev, MINOR_COUNT);
err_final:
	return err;
}

static void aaa_exit(void)
{
	device_destroy(my_class, first_dev);
	class_destroy(my_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(first_dev, MINOR_COUNT);
	pr_info("unloaded ok\n");
}

module_init(aaa_init);
module_exit(aaa_exit);
