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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <stdio.h>	// for printf(3)
#include <time.h>	// for clock(3), CLOCKS_PER_SEC
#include <assert.h>	// for assert(3)
#include <sched.h>	// for sched_yield(2)
#include <unistd.h>	// for usleep(3)
#include <sys/time.h>	// for gettimeofday(2)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <timeval_utils.h>	// for diff_timeval_in_micro()

/*
 * This is a program that measures it's own cpu usage using the
 * clock(3) API. Underneath this is actually the clock_gettime(2)
 * system call with the CLOCK_PROCESS_CPUTIME_ID.
 *
 * So in this example you are measuing real cpu time you are using and this
 * doesn't include time wasted by context switches to other processes.
 *
 * We actually make sure that wall clock time in this example will be greater
 * than CPU clock by sleeping once in a while.
 * We could also sched_yield(2) but this will only prolong wall clock time if
 * we have other processes on the same box with which we are competing for
 * CPU with.
 *
 * You can see this by using the time(1) utility to measure this program, you
 * will see real time much greater than user time.
 *
 * References:
 * - https://levelup.gitconnected.com/8-ways-to-measure-execution-time-in-c-c-48634458d0f9
 */

int main() {
	struct timeval tv_start, tv_end;
	CHECK_NOT_M1(gettimeofday(&tv_start, NULL));
	clock_t start = clock();
	double sum = 0;
	double add = 1;
	int iterations = 1000*1000*1000;
	for(int i=0; i<iterations; i++) {
		sum += add;
		if(i%10000==0) {
			usleep(1);
		}
		add /= 2.0;
	}
	assert(sum>0);
	// Stop measuring time and calculate the elapsed time
	clock_t end = clock();
	double elapsed = double(end - start)/CLOCKS_PER_SEC;
	printf("Time measured: %.3f seconds.\n", elapsed);
	CHECK_NOT_M1(gettimeofday(&tv_end, NULL));
	unsigned long diff=diff_timeval_in_micro(&tv_start, &tv_end);
	printf("Wall clock time measured: %.3f seconds.\n", (double)diff/1000.0);
	return EXIT_SUCCESS;
}
