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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer <veltzer@gnu.org>");
MODULE_DESCRIPTION("minimal Hello, World! driver doing just init and cleanup");
MODULE_VERSION("1.0.0");

static int __init hello_init(void)
{
	pr_info("in hello_init\n");
	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("in hello_exit\n");
}

module_init(hello_init);
module_exit(hello_exit);
