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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <time.h>	// for clock_gettime(2)
#include <timespec_utils.h>	// for timespec_sub()
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This is an example of measureing a simple for loop
 *
 * This is to prevent the compiler from optimizing the entire loop
 * out...
 * EXTRA_COMPILE_FLAGS_AFTER=-O0
 */

int main(int argc, char** argv, char** envp) {
	struct timespec t_start, t_end;
	CHECK_NOT_M1(clock_gettime(CLOCK_REALTIME, &t_start));
	for(int i=0; i<1000000; i++) {
	}
	/*
	 * int i=0;
	 * while(i<1000000) {
	 * i++;
	 * }
	 * printf("i is %d\n", i);
	 */
	/*
	 * long long sum=0;
	 * for(int i=0;i<1000000;i++) {
	 * sum+=i*i;
	 * }
	 */
	CHECK_NOT_M1(clock_gettime(CLOCK_REALTIME, &t_end));
	// printf("sum is %lld\n", sum);
	unsigned long long diff=timespec_diff_nano(&t_end, &t_start);
	// printf("seconds since the epoch are %ld\n", t_start.tv_sec);
	printf("time spent in the loop was %llu nanosecs\n", diff);
	return EXIT_SUCCESS;
}
