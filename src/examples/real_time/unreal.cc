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
#include <stdio.h>	// for fprintf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, atoi(3)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <sched.h>	// for sched_setscheduler(2), sched_param
#include <sys/types.h>	// for pid_t

/*
 * make a real time process return and be a regular process.
 *
 * This is similar to chrt(1) from the util-linux package which is
 * a command line utility. (although that chrt has lots of features...)
 */

int main(int argc, char** argv) {
	if(argc<2) {
		fprintf(stderr, "%s: usage: %s [pid]...\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	for(int i=1; i<argc; i++) {
		struct sched_param sp={sched_priority:0};
		pid_t pid=atoi(argv[i]);
		CHECK_NOT_M1(sched_setscheduler(pid, SCHED_OTHER, &sp));
	}
	return EXIT_SUCCESS;
}
