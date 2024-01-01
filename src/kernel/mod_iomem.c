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
#include <linux/printk.h> /* for printk functions */
#include <linux/ioport.h> /* for resources */
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */

/*
 * This is a driver to walk the iomem list and see all the resources...
 *
 * Most of the code for this example is from:
 * $KERNEL_SOURCE/kernel/resource.c
 */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("Demo module for testing");

/* our own functions */
static int __init mod_init(void)
{
	struct resource *rp;

	PR_INFO("start");
	rp = iomem_resource.child;
	while (rp) {
		pr_info("%0*llx-%0*llx\n",
			8, (unsigned long long)rp->start,
			8, (unsigned long long)rp->end
		);
		/* advance to the next resource... */
		rp = rp->sibling;
	}
	return 0;
}

static void __exit mod_exit(void)
{
	PR_INFO("start");
}


/* declaration of init/cleanup functions of this module */
module_init(mod_init);
module_exit(mod_exit);
