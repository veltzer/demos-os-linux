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
#include <stdio.h>	// for fprintf(3), stderr:object, printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <time.h>	// for clock_gettime(2), clock_nanosleep(2), CLOCK_REALTIME, CLOCK_MONOTONIC
#include <pthread_utils.h>	// for pthread_stack_prefault()
#include <timespec_utils.h>	// for timespec_add_nanos(), timespec_diff_nano(), timespec_assert_ge()
#include <sched.h>	// for sched_setscheduler(2), struct sched_param
#include <sys/mman.h>	// for mlockall(2)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <clock_utils.h>// for clock_get_by_name()

/*
 * This example explores the responsiveness of the OS.
 * This is very similar to the cyclictest(1) application but a lot simpler
 * and with less features.
 *
 * TODO:
 * - make sure that we did not have any page faults at the end of the run.
 * - add cpu affinity features like in cyclictest(1) (do the work in a
 * thread).
 * - print progress as the application is running under a master thread which
 * is not real time.
 */

int main(int argc, char** argv, char** envp) {
	if(argc!=5) {
		fprintf(stderr, "%s: usage: %s [interval] [priority] [clock] [loop]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: %s 50000 49 CLOCK_REALTIME 10000\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: %s 50000 49 CLOCK_MONOTONIC 10000\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	// get the parameters
	const unsigned long long interval=atoi(argv[1]);
	const int priority=atoi(argv[2]);
	const int clock=clock_get_by_name(argv[3]);
	const unsigned int loop=atoi(argv[4]);

	// prep code
	/* Declare ourself as a real time task */
	struct sched_param param;
	param.sched_priority=priority;
	CHECK_NOT_M1(sched_setscheduler(0, SCHED_FIFO, &param));
	/* Lock memory */
	CHECK_NOT_M1(mlockall(MCL_CURRENT|MCL_FUTURE));
	/* Pre-fault our stack - this is useless because of mlock(2) */
	// pthread_stack_prefault();

	// start loop
	/* get the current time */
	struct timespec t;
	CHECK_NOT_M1(clock_gettime(clock, &t));
	/* start after one second */
	timespec_add_nanos(&t, interval);
	int success=0;
	unsigned long long max=0;
	unsigned long long min=1024*1024*1024*1024LL;
	unsigned long long sum=0;
	for(unsigned int i=0; i<loop; i++) {
		/* wait untill next shot */
		CHECK_NOT_M1(clock_nanosleep(clock, TIMER_ABSTIME, &t, NULL));
		struct timespec now;
		CHECK_NOT_M1(clock_gettime(clock, &now));
		// timespec_assert_ge(&now, &t);
		unsigned long long diff_nanos=timespec_diff_nano(&now, &t);
		if(diff_nanos>max) {
			max=diff_nanos;
		}
		if(diff_nanos<min) {
			min=diff_nanos;
		}
		sum+=diff_nanos;
		success++;
		/* do the stuff
		 * ...
		 * calculate next shot */
		timespec_add_nanos(&t, interval);
	}
	printf("max is %llu\n", max);
	printf("min is %llu\n", min);
	printf("avg is %llu\n", sum/loop);
	return EXIT_SUCCESS;
}
