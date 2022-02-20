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
#include <stdlib.h>	// for malloc(3), rand(3), EXIT_SUCCESS, EXIT_FAILURE, atoi(3)
#include <string.h>	// for malloc(3), memset(3)
#include <time.h>	// for clock_gettime(2), clock_nanosleep(2)
#include <sched.h>	// for sched_setscheduler(2), struct sched_param
#include <sys/mman.h>	// for mlockall(2)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <timespec_utils.h>	// for timespec_add_nanos()
#include <pthread_utils.h>	// for pthread_stack_prefault()

/*
 * This example explores the CPU utilisation of doing XXXM memcpy per second.
 * You need to supply how many megs you want copied and in how many intervals.
 *
 * EXTRA_LINK_FLAGS=-lpthread -lrt
 */

/* we use 49 as the PRREMPT_RT use 50 as the priority of kernel tasklets
 * and interrupt handler by default */
const int MY_PRIORITY=49;

int main(int argc, char** argv, char** envp) {
	if(argc!=3) {
		fprintf(stderr, "%s: usage: %s [megs] [intervals]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: megs must be divisible by intervals...\n", argv[0]);
		fprintf(stderr, "%s: example: %s 90 1024\n", argv[0], argv[0]);
		fprintf(stderr, "%s: which means 90Megs in 1024 intervals\n", argv[0]);
		return EXIT_FAILURE;
	}
	unsigned int megs=atoi(argv[1]);
	unsigned int intervals=atoi(argv[2]);
	const int interval=NSEC_PER_SEC/intervals;
	const int bufsize=1024*1024*megs;
	const int transfer_size=bufsize/intervals;
	if(bufsize%intervals!=0) {
		fprintf(stderr, "%s: megs must be divisible by intervals...\n", argv[0]);
		return EXIT_FAILURE;
	}
	char* buf=(char*)malloc(bufsize);
	char* buf2=(char*)malloc(bufsize);
	char* ptr=buf;
	char* ptr2=buf;

	/* Declare ourself as a real time task */
	struct sched_param param;
	param.sched_priority=49;
	CHECK_NOT_M1(sched_setscheduler(0, SCHED_FIFO, &param));

	/* Lock memory */
	CHECK_NOT_M1(mlockall(MCL_CURRENT|MCL_FUTURE));

	/* Pre-fault our stack */
	pthread_stack_prefault();

	/* get the current time */
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	/* start after one second */
	t.tv_sec++;
	while(true) {
		/* wait untill next shot */
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

		/* do the work */
		memcpy(ptr2, ptr, transfer_size);
		ptr+=transfer_size;
		ptr2+=transfer_size;
		if(ptr-buf==bufsize) {
			ptr=buf;
			ptr2=buf2;
		}
		/* calculate next shot */
		timespec_add_nanos(&t, interval);
	}
	return EXIT_SUCCESS;
}
