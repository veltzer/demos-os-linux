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
#include <linux/module.h> /* for MODULE_*. module_* */
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */

/*
 * This is a minimal module to help disassemble linux kernel
 * memory barriers.
 *
 * This example proves that mb(), rmb() and wmb() translate
 * all to the same "lock addl $0x0,(%esp)" instruction on
 * x86. barrier() translates to nothing.
 */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("Module for mb(), rmb(), wmb(), barrier() disassembly");

/* our own functions */
static int __init mod_init(void)
{
	mb(); /* this comment is mandatory by checkpatch */
	rmb();
	wmb(); /* this comment is mandatory by checkpatch */
	barrier();
	return 0;
}


static void __exit mod_exit(void)
{
}

/* declaration of init/cleanup functions of this module */
module_init(mod_init);
module_exit(mod_exit);
