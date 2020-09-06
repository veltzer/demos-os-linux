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

/*
 * for MODULE_LICENSE(), MODULE_AUTHOR(), MODULE_DESCRIPTION(), module_init(),
 * module_exit()
 */
#include <linux/module.h>
#include <linux/slab.h> /* for kmalloc(), kfree() */
#include "connect.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("Module written in C++");

static int __init link_init(void)
{
	return cpp_init();
}

static void __exit link_exit(void)
{
	cpp_exit();
}

/*
 * static void myprintk(const char *msg)
 * {
 *	printk(msg);
 * }
 *
 * static void *mymalloc(unsigned int size)
 * {
 *	return kmalloc(size, GFP_KERNEL);
 * }
 *
 * static void myfree(void *pointer)
 * {
 *	kfree(pointer);
 * }
 */

module_init(link_init);
module_exit(link_exit);
