// SPDX-License-Identifier: GPL-2.0
/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2024 Mark Veltzer <mark.veltzer@gmail.com>
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

#include <linux/module.h> /* for MODULE_*, module_* */
#include <linux/stat.h> /* for permission bits on module_param */
#include <linux/fs.h> /* for fops */
#include <linux/device.h> /* for device_create */
#include <linux/moduleparam.h> /* for module_param, MODULE_PARM_DESC... */
#include <linux/init.h> /* for __init, __exit */
#include <linux/sched.h> /* for wait/wakeup */

#include "ioctl.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("Exercise in going to sleep and waking up");

/*
 * This is a module demonstrating how to do 'ioctl' in a kernel module
 */

/* notice these hardcoded major and minor numbers (not good!!!) */
/* the minor is automatically initialised to 0... */
const int IOCTL_MAJOR = 190;
const int IOCTL_MINOR;
DECLARE_WAIT_QUEUE_HEAD(queue);
int state;

/* these are the actual operations */
static long ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case IOCTL_SLEEP:
		state = 0;
		wait_event_interruptible(queue, state == 1);
		return 0;
	case IOCTL_WAKEUP:
		state = 1;
		wake_up(&queue);
		return 0;
	default:
		return -EFAULT;
	}
}

/* this is the operations table */
static const struct file_operations ioctl_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = ioctl,
};

/* this variable will store the class */
static struct class *my_class;
/* this variable will store the device */
static struct device *my_device;

static int __init ioctl_init(void)
{
	int err;

	pr_info("start\n");
	/* this is registering the new device operations */
	err = register_chrdev(IOCTL_MAJOR, THIS_MODULE->name, &ioctl_fops);
	if (err) {
		pr_err("unable to get major %d for %s dev\n", IOCTL_MAJOR,
				THIS_MODULE->name);
		goto err_nothing;
	}
	/* this is creating a new class (/sys/class) */
	my_class = class_create(THIS_MODULE->name);
	if (IS_ERR(my_class)) {
		pr_err("failed to create class\n");
		err = PTR_ERR(my_class);
		goto err_register;
	}
	pr_info("created class\n");
	/* and now lets auto-create a /dev/ node */
	my_device = device_create(my_class, NULL,
			MKDEV(IOCTL_MAJOR, IOCTL_MINOR),
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
 *	device_destroy(my_class, MKDEV(IOCTL_MAJOR, IOCTL_MINOR));
 */
err_class:
	class_destroy(my_class);
err_register:
	unregister_chrdev(IOCTL_MAJOR, THIS_MODULE->name);
err_nothing:
	return err;
}

static void __exit ioctl_exit(void)
{
	pr_info("start\n");
	device_destroy(my_class, MKDEV(IOCTL_MAJOR, IOCTL_MINOR));
	class_destroy(my_class);
	unregister_chrdev(IOCTL_MAJOR, THIS_MODULE->name);
	pr_info("device unloaded successfuly\n");
}

module_init(ioctl_init);
module_exit(ioctl_exit);
