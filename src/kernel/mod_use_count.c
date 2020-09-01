// SPDX-License-Identifier: GPL-2.0
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

/* #define DEBUG */
#include <linux/module.h> /* for MODULE_*, module_* */
#include <linux/fs.h> /* for fops */
#include <linux/device.h> /* for device support */
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("module with device file to demo use count");

/* static data */
static struct device *my_device;

/* fops */

/*
 * The file operations structure.
 */
static const struct file_operations my_fops = {
	.owner = THIS_MODULE,
};

#include "device.inc"
