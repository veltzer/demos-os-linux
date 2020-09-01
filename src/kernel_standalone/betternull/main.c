/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
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

#include <linux/module.h> /* for MODULE_*, module_* */
#include <linux/stat.h> /* for permission bits on module_param */
#include <linux/fs.h> /* for fops */
#include <linux/device.h> /* for device_create */
#include <linux/moduleparam.h> /* for module_param, MODULE_PARM_DESC... */
#include <linux/init.h> /* for __init, __exit */
#include <linux/cdev.h> /* for cdev_* */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("A simple implementation for something like /dev/null");

/*
 * This is an improvement of the mynull exercise...
*/

/* these are the actual operations */
static ssize_t write_null(struct file *file, const char __user *buf,
		size_t count, loff_t *ppos)
{
	/* remmember to increment the position to let the user think that
	he is actually writing something...
	*/
	pr_info("count is %zd\n", count);
	*ppos += count;
	return count;
}

/* this is the operations table */
static const struct file_operations null_fops = {
	.owner = THIS_MODULE,
	.write = write_null,
};

/* this variable will store the class */
static struct class *my_class;
/* this variable will hold our cdev struct */
static struct cdev cdev;
/* this variable will store the device */
static struct device *my_device;
/* this is the first dev_t allocated to us... */
static dev_t first_dev;
/* number of minors we need... */
static const int count = 1;
/* this is our first minor (0 by default)*/
static int first_minor;

static int __init null_init(void)
{
	int err;

	pr_info("start\n");
	/* allocate our own range of devices */
	err = alloc_chrdev_region(&first_dev, first_minor, count,
			THIS_MODULE->name);
	if (err) {
		pr_err("cannot alloc_chrdev_region\n");
		goto err_nothing;
	}
	pr_info("allocated the region\n");
	/* add the cdev structure */
	cdev_init(&cdev, &null_fops);
	err = cdev_add(&cdev, first_dev, count);
	if (err) {
		pr_err("cannot cdev_add\n");
		goto err_cdev_del;
	}
	pr_info("added the cdev\n");
	/* this is creating a new class (/sys/class) */
	my_class = class_create(THIS_MODULE, THIS_MODULE->name);
	if (IS_ERR(my_class)) {
		pr_err("failed to create class\n");
		err = PTR_ERR(my_class);
		goto err_cdev_del;
	}
	pr_info("created class\n");
	/* and now lets auto-create a /dev/ node */
	my_device = device_create(my_class, NULL, first_dev,
			"%s", THIS_MODULE->name);
	if (IS_ERR(my_device)) {
		pr_err("failed to create device\n");
		err = PTR_ERR(my_device);
		goto err_class;
	}
	pr_info("emitted message to udev to create /dev file in user space\n");
	pr_info("device loaded successfuly\n");
	return 0;
/* err_device:
	device_destroy(my_class, first_dev);
*/
err_class:
	class_destroy(my_class);
err_cdev_del:
	cdev_del(&cdev);
	unregister_chrdev_region(first_dev, count);
err_nothing:
	return err;
}

static void __exit null_exit(void)
{
	pr_info("start\n");
	device_destroy(my_class, first_dev);
	class_destroy(my_class);
	cdev_del(&cdev);
	unregister_chrdev_region(first_dev, count);
	pr_info("device unloaded successfuly\n");
}

module_init(null_init);
module_exit(null_exit);
