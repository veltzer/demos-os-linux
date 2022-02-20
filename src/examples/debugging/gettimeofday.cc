/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <sys/time.h>	// for gettimeofday(2)
#include <unistd.h>	// for usleep(3)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <stdlib.h>	// for EXIT_SUCCESS
#include <timeval_utils.h>	// for diff_timeval_in_micro(), print_timeval()

/*
 * This example shows how to use gettimeofday
 * This example also shows how accurate this method of measurement is...
 */

int main(int argc, char** argv, char** envp) {
	struct timeval tv_start, tv_end;
	CHECK_NOT_M1(gettimeofday(&tv_start, NULL));
	CHECK_NOT_M1(usleep(50 * 1000));
	CHECK_NOT_M1(gettimeofday(&tv_end, NULL));
	print_timeval(&tv_start, "start");
	print_timeval(&tv_end, "end");
	unsigned long diff=diff_timeval_in_micro(&tv_start, &tv_end);
	printf("diff is %lu\n", diff);
	return EXIT_SUCCESS;
}
