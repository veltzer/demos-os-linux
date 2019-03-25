/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <linux/module.h> /* for MODULE_*. module_* */
#include <linux/printk.h> /* for pr_* */
#include <linux/hardirq.h> /* for in_interrupt() */
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */

/*
* This module shows different contexts in the kernel.
*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("show different contexts in the kernel");

/* our own functions */
static int __init mod_init(void)
{
	if (in_interrupt())
		pr_info("in_interrupt is 1\n");
	else
		pr_info("in_interrupt is 0\n");
	return 0;
}


static void __exit mod_exit(void)
{
}

/* declaration of init/cleanup functions of this module */
module_init(mod_init);
module_exit(mod_exit);
