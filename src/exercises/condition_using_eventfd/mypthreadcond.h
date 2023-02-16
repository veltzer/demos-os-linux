/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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

#ifndef __mypthreadcond_h
#define __mypthreadcond_h

/* THIS IS A C FILE, NO C++ here */

#include <firstinclude.h>
#include <unistd.h>	// for close(2), read(2), write(2)
#include <sys/eventfd.h>// for eventfd(2)
#include <pthread.h>	// for pthread_mutex_lock(3), pthread_mutex_unlock(3)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_INT(), CHECK_NOT_M1(), CHECK_ASSERT()

// this is the condition implementation (pthread "like")
const unsigned int MAX_FD=100;
typedef struct _mypthread_cond_t {
	int efd[MAX_FD];
	bool waiting[MAX_FD];
} mypthread_cond_t;

static inline int mypthread_cond_init(mypthread_cond_t *cond) {
	for(unsigned int i=0; i<MAX_FD; i++) {
		cond->efd[i]=CHECK_NOT_M1(eventfd(0, 0));
		cond->waiting[i]=false;
	}
	return 0;
}

static inline int mypthread_cond_signal(mypthread_cond_t *cond) {
	for(unsigned int i=0; i<MAX_FD; i++) {
		if(cond->waiting[i]) {
			uint64_t u=1;
			CHECK_INT(write(cond->efd[i], &u, sizeof(uint64_t)), sizeof(uint64_t));
			break;
		}
	}
	return 0;
}

static inline int mypthread_cond_broadcast(mypthread_cond_t *cond) {
	for(unsigned int i=0; i<MAX_FD; i++) {
		if(cond->waiting[i]) {
			uint64_t u=1;
			CHECK_INT(write(cond->efd[i], &u, sizeof(uint64_t)), sizeof(uint64_t));
		}
	}
	return 0;
}

static inline int mypthread_cond_wait(mypthread_cond_t *cond, pthread_mutex_t *mutex) {
	int mynumber=-1;
	for(unsigned int i=0; i<MAX_FD; i++) {
		if(!cond->waiting[i]) {
			mynumber=i;
			break;
		}
	}
	CHECK_ASSERT(mynumber!=-1);
	cond->waiting[mynumber]=true;
	CHECK_ZERO_ERRNO(pthread_mutex_unlock(mutex));
	uint64_t u;
	CHECK_INT(read(cond->efd[mynumber], &u, sizeof(uint64_t)), sizeof(uint64_t));
	CHECK_ZERO_ERRNO(pthread_mutex_lock(mutex));
	cond->waiting[mynumber]=false;
	return 0;
}

static inline int mypthread_cond_destroy(mypthread_cond_t *cond) {
	for(unsigned int i=0; i<MAX_FD; i++) {
		CHECK_NOT_M1(close(cond->efd[i]));
	}
	return 0;
}

#endif	/* !__mypthreadcond_h */
