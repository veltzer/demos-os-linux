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
#include <linux/module.h> /* for the MODULE_*, module_* */
/* define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("Module showing how to use BUG()");

/*
*	Take note of the following facts:
*	- after a driver does a BUG() it cannot be unloaded
*	(reboot will be needed).
*	- BUG() generates a snipplet in the kernel log that can be used to
*	report
*		bugs to kernel maintainers.
*	- BUG() generates a stack trace that you can use for debugging.
*
*	TODO:
*	- examin the behaviour in each of these and describe it.
*	- add open and ioctl support.
*/
static int __init mod_init(void)
{
	BUG();
	BUG_ON(1 != 2);
	panic("there is nothing I can do");
	return 0;
}

static void __exit mod_exit(void)
{
}

module_init(mod_init);
module_exit(mod_exit);
