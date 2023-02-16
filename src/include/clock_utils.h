/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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

#ifndef __clock_utils_h
#define __clock_utils_h

/*
 * This is a collection of helper function to help with dealing
 * with the Linux clocks API
 */

/* THIS IS A C FILE, NO C++ here */

#include <firstinclude.h>
#include <stdio.h>	// for printf(3)
#include <time.h>	// for clock_getres(2), clock_gettime(2), struct timespec
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <us_helper.h>	// for ARRAY_SIZEOF()
#include <string.h>	// for strcmp(3)

/*
 * A structure containing the clock id and it's name
 */
typedef struct _clock_val_and_name {
	clockid_t val;
	const char* name;
} clock_val_and_name;

/*
 * List of all signals and their names. The reason for this is that the standard
 * C library does not provide a translation to/from clock name to/from signal value.
 * Note: you cant use stringification macros here because they will substitute
 * the macro. Use only macros that don't substitute.
 * These clock ids are taken from /usr/include/bits/time.h...
 */
#define entry(x) { x, # x }
static clock_val_and_name clock_tbl[]={
	entry(CLOCK_REALTIME),
	entry(CLOCK_MONOTONIC),
	entry(CLOCK_PROCESS_CPUTIME_ID),
	entry(CLOCK_THREAD_CPUTIME_ID),
	entry(CLOCK_MONOTONIC_RAW),
	entry(CLOCK_REALTIME_COARSE),
	entry(CLOCK_MONOTONIC_COARSE),
};
#undef entry

/*
 * Print out a table of all clock values and names
 * Notice that:
 * - different clocks show different times.
 * - different clocks have different resolutions.
 */
static inline void clock_print_table() {
	unsigned int i;
	for(i=0; i<ARRAY_SIZEOF(clock_tbl); i++) {
		clockid_t clk_id=clock_tbl[i].val;
		const char* name=clock_tbl[i].name;
		struct timespec res, ts;
		CHECK_NOT_M1(clock_getres(clk_id, &res));
		CHECK_NOT_M1(clock_gettime(clk_id, &ts));
		printf("clock resolution for clock %s (%d) is %ld (sec) %ld (nsec), time %ld.%09ld\n", name, clk_id, res.tv_sec, res.tv_nsec, ts.tv_sec, ts.tv_nsec);
	}
}

/*
 * Translate clock name to clock value
 */
static inline int clock_get_by_name(const char* name) {
	unsigned int i;
	for(i=0; i<ARRAY_SIZEOF(clock_tbl); i++) {
		if(strcmp(name, clock_tbl[i].name)==0) {
			return clock_tbl[i].val;
		}
	}
	CHECK_ERROR("bad clock name");
}
#endif	/* !__clock_utils_h */
