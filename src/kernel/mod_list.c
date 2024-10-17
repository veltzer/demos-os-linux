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
#include <linux/module.h> /* for MODULE_* */
#include <linux/fs.h> /* for fops */
#include <linux/device.h> /* for struct device */
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */
#include "shared.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("Kernel module that demostrates manipulation of list structures using kernel built in lists");

#include "list.inc"

/* static data */
static struct device *my_device;
static void *lptr;

/* fops */

/*
 * This is the ioctl implementation.
 */
static long kern_unlocked_ioctl(struct file *filp, unsigned int cmd,
		unsigned long arg)
{
	int res;
	void *p;

	pr_debug("start");
	switch (cmd) {
	case IOCTL_LIST_CREATE:
		lptr = api_list_create();
		return 0;
	case IOCTL_LIST_DESTROY:
		api_list_destroy(lptr);
		lptr = NULL;
		return 0;
	case IOCTL_LIST_ISEMPTY:
		res = api_list_isempty(lptr);
		pr_debug("res is %d", res);
		return 0;
	case IOCTL_LIST_ADD:
		api_list_add(lptr, (void *)arg);
		return 0;
	case IOCTL_LIST_DEL:
		p = api_list_del(lptr);
		pr_debug("p is %ld", (int)p);
		return 0;
	case IOCTL_LIST_PRINT:
		api_list_print(lptr);
		return 0;
	}
	return -EINVAL;
}

/*
 * The file operations structure.
 */
static const struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = kern_unlocked_ioctl,
};

#include "device.inc"
