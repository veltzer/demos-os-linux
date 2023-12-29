// SPDX-License-Identifier: GPL-2.0
/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("A skeleton for a character device drivfer");

/*
 * TODO:
 * - allocate the major and minors dynamically
 */
static int auto_register = 1;
module_param(auto_register, int, 0644);
MODULE_PARM_DESC(auto_register, "Do you want me to auto register the file in /dev ?");

/* notice these hardcoded major and minor numbers (not good!!!) */
/* minor is automatically initialised to 0... */
const int NULL_MAJOR = 190;
const int NULL_MINOR;

/* implement the operations here */

/* this is the operations table */
static const struct file_operations null_fops = {
	.owner = THIS_MODULE,
};

/* this variable will store the class */
static struct class *my_class;
static struct device *my_device;

static int __init skeleton_init(void)
{
	int err;

	pr_info("start\n");
	/* this is registering the new device operations */
	err = register_chrdev(NULL_MAJOR, THIS_MODULE->name, &null_fops);
	if (err) {
		pr_err("unable to get major %d for %s dev\n",
				NULL_MAJOR, THIS_MODULE->name);
		goto err_nothing;
	}
	if (auto_register) {
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
				MKDEV(NULL_MAJOR, NULL_MINOR), "%s",
				THIS_MODULE->name);
		if (IS_ERR(my_device)) {
			pr_err("failed to create device\n");
			err = PTR_ERR(my_device);
			goto err_class;
		}
		pr_info("emitted message to udev to create /dev file in user space\n");
	}
	pr_info("device loaded successfuly\n");
	return 0;
/*
 * err_device:
 *	device_destroy(my_class, MKDEV(NULL_MAJOR, NULL_MINOR));
 */
err_class:
	class_destroy(my_class);
err_register:
	unregister_chrdev(NULL_MAJOR, THIS_MODULE->name);
err_nothing:
	return err;
}

static void __exit skeleton_exit(void)
{
	pr_info("start\n");
	if (auto_register) {
		device_destroy(my_class, MKDEV(NULL_MAJOR, NULL_MINOR));
		class_destroy(my_class);
	}
	unregister_chrdev(NULL_MAJOR, THIS_MODULE->name);
	pr_info("device unloaded successfuly\n");
}

module_init(skeleton_init);
module_exit(skeleton_exit);
