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
module_param(myint, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
MODULE_PARM_DESC(myint, "myint controls bla bla bla...");

/* our own functions */
static int __init myparam_init(void)
{
	pr_info("in myparam_init\n");
	pr_info("in myparam_init: myint is %d\n", myint);
	return 0;
}

static void __exit myparam_exit(void)
{
	pr_info("in myparam_exit: myint is %d\n", myint);
	pr_info("in myparam_exit\n");
}

module_init(myparam_init);
module_exit(myparam_exit);
