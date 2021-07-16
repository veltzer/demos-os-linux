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

#include <firstinclude.h>
#include <stdio.h>	// for stderr, fprintf(3)
#include <stdlib.h>	// for atoi(3), EXIT_SUCCESS, EXIT_FAILURE
#include <sys/time.h>	// for gettimeofday(2)
#include <measure.h>	// for measure, measure_init(), measure_start(), measure_end(), measure_print()
#include <sched_utils.h>// for sched_run_priority(), SCHED_FIFO_HIGH_PRIORITY:const

/*
 * A solution to the timing exercise (timing gettimeofday(2) using gettimeofday(2)).
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */

unsigned int count;

void* func(void*) {
	measure m;
	// think! why the +1?
	measure_init(&m, "syscall", count+1);
	measure_start(&m);
	for(unsigned int i=0; i<count; i++) {
		struct timeval t3;
		gettimeofday(&t3, NULL);
	}
	measure_end(&m);
	measure_print(&m);
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [count]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	count=atoi(argv[1]);
	sched_run_priority(func, NULL, SCHED_FIFO_HIGH_PRIORITY, SCHED_FIFO);
	return EXIT_SUCCESS;
}
