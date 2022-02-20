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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <time.h>	// for clock_gettime(2), clock_nanosleep(2)
#include <sched.h>	// for sched_setscheduler(2), struct sched_param
#include <sys/mman.h>	// for mlockall(2)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <pthread_utils.h>	// for pthread_stack_prefault()
#include <timespec_utils.h>	// for timespec_add_nanos()

/*
 * This is a real time skeleton that shows all the critical parts of a real
 * time application in Linux.
 *
 * This is written in C so as to be incoporatable into C programs as well as
 * C++ ones.
 *
 * Kudos to Gilad Ben-Yosef for providing the basis upon which this code
 * was built.
 *
 * TODO:
 * - show that this app does not do any page faults. Do this by not having
 * it do an infinite loop and just looking at it's page faults. Or maybe
 * printing it's page faults while it is running?
 *
 * EXTRA_LINK_FLAGS=-lpthread -lrt
 */

/*
 * we use 49 as the PREEMPT_RT linux kernel patch uses 50 as the priority of
 * kernel tasklets and interrupt handlers by default
 */
const int MY_PRIORITY=49;
/* The interval size (50us which is 50000ns in our case) */
const int interval=50000;

int main(int argc, char** argv, char** envp) {
	/* Declare ourself as a real time task */
	struct sched_param param;
	param.sched_priority=MY_PRIORITY;
	CHECK_NOT_M1(sched_setscheduler(0, SCHED_FIFO, &param));
	/* Lock memory */
	CHECK_NOT_M1(mlockall(MCL_CURRENT|MCL_FUTURE));
	/* Pre-fault our stack - this is useless because of mlock(2) */
	pthread_stack_prefault();
	/* get the current time */
	struct timespec t;
	CHECK_NOT_M1(clock_gettime(CLOCK_MONOTONIC, &t));
	/* calculate first shot */
	timespec_add_nanos(&t, interval);
	while(true) {
		/* wait until next shot */
		CHECK_NOT_M1(clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL));
		/* do your stuff
		 * ...
		 * calculate next shot */
		timespec_add_nanos(&t, interval);
	}
	return EXIT_SUCCESS;
}
