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

#ifndef __prog_utils_h
#define __prog_utils_h

/*
 * A collection of utilities to help with progress reports.
 */

/* THIS IS A C FILE, NO C++ here */

#include <firstinclude.h>
#include <stdio.h>	// for printf(3), fflush(3), stdout

/*
 * Initialize the progress meter
 */
static inline void do_prog_init(void) {
	printf("progress: \n");
	fflush(stdout);
}

/*
 * Do one progress
 */
static inline void do_prog(unsigned int i, unsigned int mod, unsigned int full) {
	if (i % mod==0) {
		printf("\r\t%d/%d", i, full);
		fflush(stdout);
	}
}

/*
 * Finish the progress reports
 */
static inline void do_prog_finish(void) {
	printf("\tfinished...\n");
	fflush(stdout);
}

#endif	/* !__prog_utils_h */
