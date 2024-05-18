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

#include <firstinclude.h>
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for malloc(3), EXIT_SUCCESS
#include <sys/time.h>	// for setrlimit(2)
#include <sys/resource.h>	// for setrlimit(2)
#include <err_utils.h>	// for CHECK_NOT_NULL(), CHECK_NOT_M1()

/*
 * This example explores the use of limits in source code.
 * You can limit the size of your programs RAM
 * and crash long before you get into swapping (recommended).
 * Many more types of limits exist. Try to use as much of them as possible.
 * set setrlimit(2) for more details.
 *
 * Note that if you do not set a limit on your memory size then on a 64 bit
 * system you can allocate ludicrous amounts of ram (131,000 GB anyone?).
 *
 * Setting of limits is controlled:
 * - at login time from /etc/security/{limits.conf,limits.d}
 * - from the command line using such shell builtins as ulimit(1) of bash (here you can only lower limits).
 * - from your source code (here you can only lower limits).
 * This is an example of the third option.
 */

int main() {
	const size_t size_to_alloc=1024*1024*1024;
	const char* desc="gigs";
	const int max_gigs=50;
	struct rlimit rlim;
	rlim.rlim_max=max_gigs*size_to_alloc;
	rlim.rlim_cur=max_gigs*size_to_alloc;
	CHECK_NOT_M1(setrlimit(RLIMIT_AS, &rlim));
	int d=0;
	while(true) {
		CHECK_NOT_NULL(malloc(size_to_alloc));
		printf("managed to allocate %d %s\n", d, desc);
		d++;
	}
	return EXIT_SUCCESS;
}
