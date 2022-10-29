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
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	// for sleep(3), nice(3)
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sched_utils.h>// for sched_print_info()
#include <err_utils.h>	// for CHECK_ZERO(), CHECK_NOT_M1()
#include <pthread_utils.h>	// for gettid()

/*
 * This example explores how to use thread priorities
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 *
 * TODO:
 * - show that high priority threads preempt lower priority.
 * - do another example like this but for SCHED_OTHER threads just
 * like I did for SCHED_OTHER processes.
 */

static pthread_t hpt;
static pthread_t mpt;
static pthread_t lpt;

static int lp=2;
static int mp=1;
static int hp=0;

static pthread_barrier_t mybarrier;

static int min_priority=0;

static void *thread_body(void *arg) {
	pid_t tid=gettid();
	int pri=*(int *)arg;
	CHECK_NOT_M1(nice(pri));
	printf("thread %d starting\n", tid);
	printf("pri is %d\n", pri);
	while(true) {
		// pthread_barrier_wait(&mybarrier);
		sched_print_info();
		CHECK_ZERO(sleep(10));
	}
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	struct sched_param my_param;
	pthread_attr_t hp_attr;
	pthread_attr_t mp_attr;
	pthread_attr_t lp_attr;

	/* MAIN-THREAD WITH LOW PRIORITY */
	my_param.sched_priority=CHECK_NOT_M1(sched_get_priority_min(SCHED_FIFO));
	CHECK_ZERO_ERRNO(pthread_setschedparam(pthread_self(), SCHED_RR, &my_param));
	sched_print_info();
	/* SCHEDULING POLICY AND PRIORITY FOR OTHER THREADS */
	CHECK_ZERO_ERRNO(pthread_attr_init(&lp_attr));
	CHECK_ZERO_ERRNO(pthread_attr_init(&mp_attr));
	CHECK_ZERO_ERRNO(pthread_attr_init(&hp_attr));
	CHECK_ZERO_ERRNO(pthread_attr_setinheritsched(&lp_attr, PTHREAD_EXPLICIT_SCHED));
	CHECK_ZERO_ERRNO(pthread_attr_setinheritsched(&mp_attr, PTHREAD_EXPLICIT_SCHED));
	CHECK_ZERO_ERRNO(pthread_attr_setinheritsched(&hp_attr, PTHREAD_EXPLICIT_SCHED));
	CHECK_ZERO_ERRNO(pthread_attr_setschedpolicy(&lp_attr, SCHED_FIFO));
	CHECK_ZERO_ERRNO(pthread_attr_setschedpolicy(&mp_attr, SCHED_FIFO));
	CHECK_ZERO_ERRNO(pthread_attr_setschedpolicy(&hp_attr, SCHED_FIFO));
	my_param.sched_priority=min_priority + 1;
	CHECK_ZERO_ERRNO(pthread_attr_setschedparam(&lp_attr, &my_param));
	my_param.sched_priority=min_priority + 2;
	CHECK_ZERO_ERRNO(pthread_attr_setschedparam(&mp_attr, &my_param));
	my_param.sched_priority=min_priority + 3;
	CHECK_ZERO_ERRNO(pthread_attr_setschedparam(&hp_attr, &my_param));
	CHECK_ZERO_ERRNO(pthread_barrier_init(&mybarrier, NULL, 1));
	CHECK_ZERO_ERRNO(pthread_create(&lpt, NULL, thread_body, &lp));
	CHECK_ZERO_ERRNO(pthread_create(&mpt, NULL, thread_body, &mp));
	CHECK_ZERO_ERRNO(pthread_create(&hpt, NULL, thread_body, &hp));
	CHECK_ZERO_ERRNO(pthread_join(hpt, NULL));
	CHECK_ZERO_ERRNO(pthread_join(mpt, NULL));
	CHECK_ZERO_ERRNO(pthread_join(lpt, NULL));
	return EXIT_SUCCESS;
}
