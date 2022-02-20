// SPDX-License-Identifier: GPL-2.0
/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <linux/module.h> /* for MODULE_*. module_* */
#include <linux/printk.h> /* for pr_* */
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */

/*
 * Which module name is better to use: KBUILD_MODNAME or THIS_MODULE->name?
 * Probably KBUILD_MODNAME. Look at this:
 *
 * mark@abel:~/install/linux-3.1.6/drivers$
 * fgrep "KBUILD_MODNAME" `find . -type f -name "*.c"` | wc -l
 * 586
 * mark@abel:~/install/linux-3.1.6/drivers$
 * fgrep "THIS_MODULE->name" `find . -type f -name "*.c"` | wc -l
 * 1
 * mark@abel:~/install/linux-3.1.6/drivers$
 * This sort of solves the issue.
 */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("A module showing how to refer to the name of the module you are in");

/* init/exit functions */
static int __init mod_init(void)
{
	pr_info("this module is called " KBUILD_MODNAME "\n");
	pr_info("this module is called %s\n", KBUILD_MODNAME);
	pr_info("this module is called %s\n", THIS_MODULE->name);
	return 0;
}

static void __exit mod_exit(void)
{
}

/* declaration of init/cleanup functions of this module */
module_init(mod_init);
module_exit(mod_exit);
