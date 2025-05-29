// SPDX-License-Identifier: GPL-2.0
/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <linux/module.h> /* for MODULE_*, module_* */
#include <linux/timer.h> /* for timer_setup, mod_timer, del_timer */
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("Demo of the regular linux kernel timer API");

/*
 * This module shows how to use timers (non-hi-res) in the linux kernel.
 *
 * References:
 * http://www.ibm.com/developerworks/linux/library/l-timers-list/
 */

static struct timer_list my_timer;

static void my_timer_callback(struct timer_list *list)
{
	pr_info("%s called (%ld).\n", __func__, jiffies);
}

static int __init mod_init(void)
{
	int ret;

	pr_info("Timer module installing\n");
	timer_setup(&my_timer, my_timer_callback, 0);
	pr_info("Starting timer to fire in 200ms (%ld)\n", jiffies);
	ret = mod_timer(&my_timer, jiffies+msecs_to_jiffies(200));
	if (ret) {
		pr_err("Error in mod_timer\n");
		return ret;
	}
	return 0;
}

static void __exit mod_exit(void)
{
	int ret;

	ret = del_timer(&my_timer);
	if (ret)
		pr_err("The timer is still in use...\n");
	pr_info("Timer module uninstalling\n");
}

module_init(mod_init);
module_exit(mod_exit);
