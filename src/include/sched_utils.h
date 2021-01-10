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

#ifndef __sched_utils_h
#define __sched_utils_h

/*
 * This is a set of utilities to help you with the linux scheduler.
 */

/* THIS IS A C FILE, NO C++ here */

#include <firstinclude.h>
#include <us_helper.h>	// for ARRAY_SIZE()
#include <err_utils.h>	// for CHECK_ASSERT(), CHECK_ZERO_ERRNO(), CHECK_NOT_M1()
#include <string.h>	// for strcmp(3)
#include <sched.h>	// for sched_param:struct, sched_getparam(2), sched_getscheduler(2), sched_get_priority_max(2), sched_get_priority_min(2)
#include <stdio.h>	// for printf(3)
#include <pthread.h>	// for pthread_attr_t:struct, pthread_t:struct, pthread_attr_init(3), pthread_attr_setinheritsched(3), pthread_attr_setschedpolicy(3), pthread_attr_setschedparam(3), pthread_create(3), pthread_join(3)

/*
 * Print the scheduling info for the current thread or process
 */
typedef struct _sched_val_and_name {
	int val;
	const char* name;
} sched_val_and_name;

/*
 * List of all schedulers, their names and values
 * see sched_getscheduler(2) for more details about all these constants.
 */
#define entry(x) { x, # x }
static sched_val_and_name sched_tbl[]={
	entry(SCHED_OTHER),
	entry(SCHED_FIFO),
	entry(SCHED_RR),
	entry(SCHED_BATCH),
	entry(SCHED_IDLE),
};
#undef entry

static inline int sched_get_by_name(const char* name) {
	unsigned int i;
	for(i=0; i<ARRAY_SIZEOF(sched_tbl); i++) {
		if(strcmp(name, sched_tbl[i].name)==0) {
			return sched_tbl[i].val;
		}
	}
	CHECK_ERROR("bad policy name");
}

static inline const char* sched_get_name(int policy) {
	unsigned int i;
	for(i=0; i<ARRAY_SIZEOF(sched_tbl); i++) {
		if(sched_tbl[i].val==policy) {
			return sched_tbl[i].name;
		}
	}
	CHECK_ERROR("bad policy value");
}

static inline void sched_print_info() {
	// 0 means the calling thread, process or process group
	// int pri=getpriority(PRIO_PROCESS, 0);
	// printf("getpriority %d\n", tid, pri);
	struct sched_param myparam;
	int scheduler;
	// 0 means current process
	CHECK_NOT_M1(sched_getparam(0, &myparam));
	scheduler=CHECK_NOT_M1(sched_getscheduler(0));
	printf("==================================\n");
	printf("scheduling data for the current thread...\n");
	printf("sched_getparam returned %d\n", myparam.sched_priority);
	printf("sched_getscheduler returned %s\n", sched_get_name(scheduler));
	printf("==================================\n");
}

/*
 * print parameters of the scheduling system
 */
static inline void sched_print_table() {
	unsigned int i;
	for(i=0; i<ARRAY_SIZEOF(sched_tbl); i++) {
		int val=sched_tbl[i].val;
		const char* name=sched_tbl[i].name;
		printf("%s is %d (min=%d, max=%d)\n", name, val,
			CHECK_NOT_M1(sched_get_priority_min(val)),
			CHECK_NOT_M1(sched_get_priority_max(val))
			);
	}
}
/*
 * Check that an integer is indeed a policy
 */
static inline void sched_check_policy(int policy) {
	unsigned int i;
	for(i=0; i<ARRAY_SIZEOF(sched_tbl); i++) {
		if(policy==sched_tbl[i].val) {
			return;
		}
	}
	CHECK_ERROR("bad policy value");
}

const int SCHED_FIFO_LOW_PRIORITY=1;
const int SCHED_FIFO_MID_PRIORITY=47;
const int SCHED_FIFO_HIGH_PRIORITY=90;

/*
 * a function to run another function in a high priority thread and wait for it to finish...
 * TODO:
 * - check that we indeed do not context switch out during the measurements.
 * this can be done using /proc/self/status and the nonvoluntary_ctxt_switches
 * parameter.
 *
 */
static inline void* sched_run_priority(void* (*func)(void*), void* pval, int prio, int policy) {
	sched_check_policy(policy);
	struct sched_param myparam;
	void* retval;
	pthread_attr_t myattr;
	pthread_t mythread;
	if (policy==SCHED_FIFO) {
		myparam.sched_priority=prio;
		pthread_attr_init(&myattr);
		pthread_attr_setinheritsched(&myattr, PTHREAD_EXPLICIT_SCHED);
		pthread_attr_setschedpolicy(&myattr, SCHED_FIFO);
		pthread_attr_setschedparam(&myattr, &myparam);
		CHECK_ZERO_ERRNO(pthread_create(&mythread, &myattr, func, pval));
	} else {
		CHECK_ZERO_ERRNO(pthread_create(&mythread, NULL, func, pval));
	}
	CHECK_ZERO_ERRNO(pthread_join(mythread, &retval));
	return retval;
}

#endif	/* !__sched_utils_h */
