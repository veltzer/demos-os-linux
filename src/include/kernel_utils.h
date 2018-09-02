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

#ifndef __kernel_utils_h
#define __kernel_utils_h

/*
 * This is a collection of helper function to help with kernel
 * user/space stuff.
 */

/* THIS IS A C FILE, NO C++ here */

#include <firstinclude.h>
#include <proc_utils.h>	// for my_system()

// kernel log handling functions
static inline void klog_clear(void) {
	my_system("sudo dmesg -c > /dev/null");
}

static inline void klog_show(void) {
	my_system("sudo dmesg");
}

static inline void klog_show_clear(void) {
	klog_show();
	klog_clear();
}

#endif	/* !__kernel_utils_h */
