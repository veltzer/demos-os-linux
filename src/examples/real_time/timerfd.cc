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
#include <sys/timerfd.h>// for timerfd_create(2), timerfd_settime(2), timerfd_gettime(2)
#include <unistd.h>	// for read(2)
#include <time.h>	// for clock_gettime(2)
#include <stdlib.h>	// for EXIT_FAILURE, EXIT_SUCCESS
#include <stdio.h>	// for printf(3), fprintf(3)
#include <stdint.h>	// for uint64_t
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <sched_utils.h>// for sched_run_priority(), sched_get_by_name(), SCHED_FIFO_HIGH_PRIORITY:const
#include <timespec_utils.h>	// for timespec_set(), timespec_add_secs(), timespec_sub(), timespec_snprintf()

/*
 * This example was stolen shamelessly from the timerfd_create(2) manpage,
 * and adapted to my coding style.
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */

static void print_current_time(struct timespec* since) {
	struct timespec current_time;
	CHECK_NOT_M1(clock_gettime(CLOCK_REALTIME, &current_time));
	char buf[1024];
	timespec_snprintf(buf, sizeof(buf), &current_time, 1);
	printf("current: %s\n", buf);
	struct timespec diff;
	timespec_sub(&diff, &current_time, since);
	timespec_snprintf(buf, sizeof(buf), &diff, 0);
	printf("diff: %s\n", buf);
}

typedef struct _thread_data {
	int init_sec;
	int init_nsec;
	int interval_sec;
	int interval_nsec;
	uint64_t max_exp;
} thread_data;

void* work(void* data) {
	// get the thread data
	thread_data* td=(thread_data*)data;
	// get the current time
	struct timespec start;
	CHECK_NOT_M1(clock_gettime(CLOCK_REALTIME, &start));
	/*
	 * Create a CLOCK_REALTIME absolute timer with initial
	 * expiration and interval as specified in command line
	 */
	struct itimerspec new_value;
	// If you want an absolute timer
	// timespec_copy(&new_value.it_value,&now);
	timespec_set(&new_value.it_value, td->init_sec, td->init_nsec);
	timespec_set(&new_value.it_interval, td->interval_sec, td->interval_nsec);

	int fd=CHECK_NOT_M1(timerfd_create(CLOCK_REALTIME, 0));
	// If you want an absolute timer
	// CHECK_NOT_M1(timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_value, NULL));
	CHECK_NOT_M1(timerfd_settime(fd, 0, &new_value, NULL));
	print_current_time(&start);
	printf("timer started\n");
	uint64_t tot_exp=0;
	while (tot_exp<td->max_exp) {
		uint64_t exp;
		CHECK_INT(read(fd, &exp, sizeof(uint64_t)), sizeof(uint64_t));
		tot_exp+=exp;
		print_current_time(&start);
		printf("read: %llu; total=%llu\n", (unsigned long long) exp, (unsigned long long) tot_exp);
	}
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	if (argc!=7) {
		fprintf(stderr, "%s: usage: %s [init_sec] [init_nsec] [interval_sec] [interval_nsec] [max_exp] [real_time]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: %s 2 500000000 1 500000000 6 SCHED_FIFO\n", argv[0], argv[0]);
		fprintf(stderr, "%s: and compare with:\n", argv[0]);
		fprintf(stderr, "%s: example: %s 2 500000000 1 500000000 6 SCHED_OTHER\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	// parameters
	thread_data td;
	td.init_sec=atoi(argv[1]);
	td.init_nsec=atoi(argv[2]);
	td.interval_sec=atoi(argv[3]);
	td.interval_nsec=atoi(argv[4]);
	td.max_exp=atoi(argv[5]);
	int policy=sched_get_by_name(argv[6]);
	sched_run_priority(work, &td, SCHED_FIFO_HIGH_PRIORITY, policy);
	return EXIT_SUCCESS;
}
