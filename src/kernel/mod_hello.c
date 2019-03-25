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
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */

/*
* This is a minimal module doing just init and cleanup
*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("Demo module for testing");

/* our own functions */
static int __init mod_init(void)
{
	pr_info("start\n");
	PR_INFO("start\n");
	return 0;
}


static void __exit mod_exit(void)
{
	PR_INFO("start\n");
	pr_info("start\n");
}

/* declaration of init/cleanup functions of this module */
module_init(mod_init);
module_exit(mod_exit);
