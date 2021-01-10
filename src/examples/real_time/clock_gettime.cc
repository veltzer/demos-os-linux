/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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

#include <firstinclude.h>
#include <stdio.h>	// for printf(3), fflush(3)
#include <time.h>	// for clock_getres(2)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * Demo the clock_gettime usage
 *
 * EXTRA_LINK_FLAGS=-lrt
 */

void print_timespec(const char *msg, struct timespec *ts) {
	printf("%s %ld %ld\r", msg, ts->tv_sec, ts->tv_nsec);
	fflush(stdout);
}

int main(int argc, char** argv, char** envp) {
	// clockid_t clk_id=CLOCK_REALTIME;
	clockid_t clk_id=CLOCK_MONOTONIC;
	struct timespec res;

	CHECK_NOT_M1(clock_getres(clk_id, &res));
	print_timespec("clock resolution is ", &res);
	printf("\n");
	while(true) {
		struct timespec t;
		CHECK_NOT_M1(clock_gettime(clk_id, &t));
		print_timespec("clock time is ", &t);
	}
	return EXIT_SUCCESS;
}
