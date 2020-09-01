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
#include <linux/err.h> /* for IS_ERR_VALUE */
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("Showing how IS_ERR_VALUE works");

/*
* This module shows that very high values of pointers (as unsigned longs) are
* used to pass errors around in the kernel using the IS_ERR_VALUE macro.
* This means that if there is a function that is supposed to return only
* an address it will return a very high one and you would check whether it
* encountered an error using this macro.
* Same this goes for IS_ERR and real pointers (not unsigned longs).
* You can also extract the exact error from the pointer using the PTR_ERR
* inline function.
* You can also see from this example how to create a pointer that embeds an
* error using the ERR_PTR inline function.
*
* Have a look at $KERNEL_SOURCES/include/linux/err.h for more details.
*/

static int __init mod_init(void)
{
	int i;
	for (i = -10; i < 10; i++) {
		unsigned long ptr = (unsigned long)i;
		void *p = (void *)i;
		PR_INFO("IS_ERR_VALUE(%lu) is %ld, and PTR_ERR is %ld\n", ptr,
				IS_ERR_VALUE(ptr), PTR_ERR((void *)ptr));
		PR_INFO("IS_ERR(%p) is %ld, and PTR_ERR is %ld\n", p,
				IS_ERR(p), PTR_ERR(p));
	}
	PR_INFO("ERR_PTR(-EIO) is %p", ERR_PTR(-EIO));
	return 0;
}

static void __exit mod_exit(void)
{
}

module_init(mod_init);
module_exit(mod_exit);
