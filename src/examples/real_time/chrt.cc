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

#include <firstinclude.h>
#include <sched.h>	// for sched_param, sched_setscheduler(2), SCHED_FIFO
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, atoi(3)
#include <stdio.h>	// for fprintf(3), printf(3), stderr:object
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <unistd.h>	// for execvp(3)

/*
 * This is an example of a utility program to run any program under real time priority
 *
 * This is a degenerated version of the chrt(1) command line utility from the util-linux
 * package which has better features.
 *
 * The next is needed for the C99 like struct field initialization syntax below...
 * EXTRA_COMPILE_FLAGS=-std=gnu++11
 */

int main(int argc, char** argv, char** envp) {
	if(argc<3) {
		fprintf(stderr, "%s: usage: %s priority program [parameters...]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	const int priority=atoi(argv[1]);
	const char* program=argv[2];
	char* const* params=argv+2;
	if(priority<0 || priority>99) {
		fprintf(stderr, "%s: priority must be between 0 - 99!\n", argv[0]);
		return EXIT_FAILURE;
	}
	struct sched_param sched {
		.sched_priority=priority
	};
	CHECK_NOT_M1(sched_setscheduler(0, SCHED_FIFO, &sched));
	printf("%s: executing %s in realtime priority %d...\n", argv[0], argv[2], sched.sched_priority);
	CHECK_NOT_M1(execvp(program, params));
	return EXIT_SUCCESS;
}
