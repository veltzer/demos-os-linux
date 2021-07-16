// SPDX-License-Identifier: GPL-2.0
/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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

#include <linux/module.h> /* for MODULE_*, module_* */
#include <linux/printk.h> /* for printk and pr_* APIs */
#include <linux/init.h> /* for __init, __exit */
#include <linux/moduleparam.h> /* for module_param(), MODULE_PARM_DESC() */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("A module showing off module parameters");
MODULE_VERSION("2.4.6");

/* here comes the parameters */

static int myint = 1;
module_param(myint, int, 0664);
MODULE_PARM_DESC(myint, "myint controls bla bla bla...");

/* our own functions */
static int __init myparam_init(void)
{
	pr_info("%s\n", __func__);
	pr_info("%s: myint is %d\n", __func__, myint);
	return 0;
}

static void __exit myparam_exit(void)
{
	pr_info("%s: myint is %d\n", __func__, myint);
	pr_info("%s\n", __func__);
}

module_init(myparam_init);
module_exit(myparam_exit);
