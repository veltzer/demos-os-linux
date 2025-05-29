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

/* #define DEBUG */
#include <linux/module.h> /* for MODULE_* */
#include <linux/device.h> /* for dev_[warn|info|..] */
#include <linux/fs.h> /* for fops */
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */

/*
 * This is a driver which prints stuff at open and release and allows you
 * to explore how open and close in user space translate to open and release
 * calls in the kernel
 */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("module to investigate fork(2) behaviour");

/* static data */
static struct device *my_device;

/* fops */

/*
 * The open implementation. Currently this does nothing
 */
static int kern_open(struct inode *inode, struct file *filp)
{
	dev_info(my_device, "open start");
	dev_info(my_device, "open end");
	return 0;
}

/*
 * The release implementation. Currently this does nothing
 */
static int kern_release(struct inode *inode, struct file *filp)
{
	dev_info(my_device, "release start");
	dev_info(my_device, "release end");
	return 0;
}

static ssize_t kern_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	*ppos += count;
	dev_info(my_device, "position %lld, filp %p", *ppos, file);
	return count;
}

/*
 * The file operations structure.
 */
static const struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.open = kern_open,
	.release = kern_release,
	.read = kern_read,
};

#include "device.inc"
