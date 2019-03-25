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
#include <linux/delay.h> /* for msleep() */
#include "api.h" /* for api_call() */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("Multi file kernel module example");

/* our own functions */
static int __init mod_init(void)
{
	int i;
	/* call api_call 10 times */
	for (i = 0; i < 10; i++) {
		api_call();
		msleep(2000);
	}
	return 0;
}

static void __exit mod_exit(void)
{
}

/* declaration of init/cleanup functions of this module */
module_init(mod_init);
module_exit(mod_exit);
