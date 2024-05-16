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
#include <linux/module.h> /* for MODULE_*, module_* */
#include <linux/hrtimer.h> /* for hrtimer_init, hrtimer_stat, hrtimer_cancel */
#include <linux/ktime.h> /* for ktime_set */
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("Demo of the high resolution timer API");

/*
 * This module shows how to use hi res timers in the linux kernel.
 *
 * References:
 * http://www.ibm.com/developerworks/linux/library/l-timers-list/
 */

#define MS_TO_NS(x) (x*1E6L)

static struct hrtimer hr_timer;

static enum hrtimer_restart my_hrtimer_callback(struct hrtimer *timer) {
	pr_info("%s called (%ld).\n", __func__, jiffies);
	return HRTIMER_NORESTART;
}

static int __init mod_init(void) {
	ktime_t ktime;
	unsigned long delay_in_ms = 200L;

	pr_info("HR Timer module installing\n");
	ktime = ktime_set(0, MS_TO_NS(delay_in_ms));
	hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hr_timer.function = &my_hrtimer_callback;
	pr_info("Starting timer to fire in %ldms (%ld)\n",
			delay_in_ms, jiffies);
	hrtimer_start(&hr_timer, ktime, HRTIMER_MODE_REL);
	return 0;
}

static void __exit mod_exit(void) {
	int ret;

	ret = hrtimer_cancel(&hr_timer);
	if (ret)
		pr_info("The timer was still in use...\n");
	pr_info("HR Timer module uninstalling\n");
}

module_init(mod_init);
module_exit(mod_exit);
