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

/* #define DEBUG */
#include <linux/module.h> /* for MODULE_*, module_* */
#include <linux/fs.h> /* for fops */
#include <linux/device.h> /* for struct device and dev_* functions */
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("modules that shows how to use the dev_* debugging macros");

/* static data */
static struct device *my_device;

/* fops */

/*
 * The open implementation.
 */
static int kern_open(struct inode *inode, struct file *filp) {
	/* you can use dev_printk like you would printk only with the
	 * added device...
	 */
	dev_dbg(my_device, "this is my debug message");
	/* or better yet, use the predefined ones:
	 */
	dev_emerg(my_device, "emergency");
	dev_alert(my_device, "alert");
	dev_crit(my_device, "critical");
	dev_err(my_device, "error");
	dev_warn(my_device, "warning");
	dev_notice(my_device, "notice");
	dev_info(my_device, "info");
	return 0;
}

/*
 * The file operations structure.
 */
static const struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.open = kern_open,
};

#include "device.inc"
