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

#pragma once

/*
 * This is a collection of helper function to help with working.
 *
 * TODO:
 * - make the do_work() function actually watch the clock and not
 * do more than seconds of work.
 */

/* THIS IS A C FILE, NO C++ here */

#include <firstinclude.h>

#pragma GCC push_options
#pragma GCC optimize ("O0")
static inline void do_work(int seconds __attribute__((unused))) {
	int __attribute__((unused)) i, j;
	for(int i=0; i<1000000; i++) {
		for(int j=0; j<1000000; j++) {
		}
	}
}
#pragma GCC pop_options
