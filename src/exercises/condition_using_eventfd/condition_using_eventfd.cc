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

#include <firstinclude.h>
#include <stdio.h>	// for fprintf(3)
#include <pthread.h>	// for pthread_create(3), pthread_join(3), pthread_attr_init(3), pthread_attr_setaffinity_np(3)
#include <unistd.h>	// for sysconf(3), usleep(3)
#include <sched.h>	// for CPU_ZERO(3), CPU_SET(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_NOT_M1()
#include "mypthreadcond.h"	// for mypthread_cond_init(), mypthread_cond_destroy(), mypthread_cond_wait(), mypthread_cond_signal(), my_pthread_cond_broadcast()

/*
 * This is a solution to the pthread condition lock exercise.
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

// this is the spin lock implementation (pthread "like")
typedef struct _mypthread_rwlock_t {
	mypthread_cond_t mycond_readers;
	mypthread_cond_t mycond_writers;
	pthread_mutex_t mymutex;
	unsigned int readers;
	unsigned int writers;
	unsigned int readers_waiting;
	unsigned int writers_waiting;
} mypthread_rwlock_t;

int mypthread_rwlock_init(mypthread_rwlock_t* lock) {
	CHECK_ZERO(mypthread_cond_init(&lock->mycond_readers));
	CHECK_ZERO(mypthread_cond_init(&lock->mycond_writers));
	CHECK_ZERO_ERRNO(pthread_mutex_init(&lock->mymutex, NULL));
	lock->readers=0;
	lock->writers=0;
	lock->readers_waiting=0;
	lock->writers_waiting=0;
	return 0;
}
int mypthread_rwlock_destroy(mypthread_rwlock_t* lock) {
	CHECK_ZERO(mypthread_cond_destroy(&lock->mycond_readers));
	CHECK_ZERO(mypthread_cond_destroy(&lock->mycond_writers));
	CHECK_ZERO_ERRNO(pthread_mutex_destroy(&lock->mymutex));
	return 0;
}
int mypthread_rwlock_rdlock(mypthread_rwlock_t* lock) {
	CHECK_ZERO_ERRNO(pthread_mutex_lock(&lock->mymutex));
	lock->readers_waiting++;
	// watch out! next line prevents starvation...
	while(lock->writers>0 || lock->writers_waiting>0) {
		CHECK_ZERO(mypthread_cond_wait(&lock->mycond_readers, &lock->mymutex));
	}
	lock->readers++;
	lock->readers_waiting--;
	CHECK_ZERO_ERRNO(pthread_mutex_unlock(&lock->mymutex));
	return 0;
}
int mypthread_rwlock_wrlock(mypthread_rwlock_t* lock) {
	CHECK_ZERO_ERRNO(pthread_mutex_lock(&lock->mymutex));
	lock->writers_waiting++;
	while(lock->readers>0 || lock->writers>0) {
		CHECK_ZERO(mypthread_cond_wait(&lock->mycond_writers, &lock->mymutex));
	}
	lock->writers++;
	lock->writers_waiting--;
	CHECK_ZERO_ERRNO(pthread_mutex_unlock(&lock->mymutex));
	return 0;
}
int mypthread_rwlock_unlock(mypthread_rwlock_t* lock) {
	CHECK_ZERO_ERRNO(pthread_mutex_lock(&lock->mymutex));
	if(lock->readers) {
		// I am a reader
		lock->readers--;
		// the lock->readers==0 is redundant in the next line
		if(lock->writers_waiting>0 && lock->readers==0) {
			// we use _signal because we want to wake up only one writer
			CHECK_ZERO(mypthread_cond_signal(&lock->mycond_writers));
		} else {
			// the lock->writers==0 is redundant in the next line
			if(lock->readers_waiting>0 && lock->writers==0) {
				CHECK_ZERO(mypthread_cond_broadcast(&lock->mycond_readers));
			}
		}
	} else {
		// I am a writer
		lock->writers--;
		// the lock->readers==0 is redundant in the next line
		if(lock->writers_waiting>0 && lock->readers==0) {
			// we use _signal because we want to wake up only one writer
			CHECK_ZERO(mypthread_cond_signal(&lock->mycond_writers));
		} else {
			// the lock->writers==0 is redundant in the next line
			if(lock->readers_waiting>0 && lock->writers==0) {
				CHECK_ZERO(mypthread_cond_broadcast(&lock->mycond_readers));
			}
		}
	}
	CHECK_ZERO_ERRNO(pthread_mutex_unlock(&lock->mymutex));
	return 0;
}

// testing starts here...

typedef struct _thread_data {
	unsigned int num;
	mypthread_rwlock_t* lock;
	unsigned int max_sleep;
	unsigned int loops;
	bool reader;
} thread_data;

static unsigned int readers=0;
static unsigned int writers=0;

void *worker(void *p) {
	thread_data* td=(thread_data *)p;
	if(td->reader) {
		for(unsigned int i=0; i<td->loops; i++) {
			CHECK_ZERO(mypthread_rwlock_rdlock(td->lock));
			CHECK_ASSERT(writers==0);
			__sync_add_and_fetch(&readers, 1);
			CHECK_NOT_M1(usleep(rand()%td->max_sleep));
			__sync_sub_and_fetch(&readers, 1);
			CHECK_ZERO(mypthread_rwlock_unlock(td->lock));
			CHECK_NOT_M1(usleep(rand()%td->max_sleep));
		}
	} else {
		for(unsigned int i=0; i<td->loops; i++) {
			CHECK_ZERO(mypthread_rwlock_wrlock(td->lock));
			CHECK_ASSERT(readers==0);
			CHECK_ASSERT(writers==0);
			__sync_add_and_fetch(&writers, 1);
			CHECK_NOT_M1(usleep(rand()%td->max_sleep));
			__sync_sub_and_fetch(&writers, 1);
			CHECK_ZERO(mypthread_rwlock_unlock(td->lock));
			CHECK_NOT_M1(usleep(rand()%td->max_sleep));
		}
	}
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	const int loops=10000;
	const int max_sleep=100;
	const int cpu_num=CHECK_NOT_M1(sysconf(_SC_NPROCESSORS_ONLN));
	const int thread_num=cpu_num;
	pthread_t* threads=new pthread_t[thread_num];
	pthread_attr_t* attrs=new pthread_attr_t[thread_num];
	thread_data* tds=new thread_data[thread_num];
	cpu_set_t* cpu_sets=new cpu_set_t[thread_num];

	mypthread_rwlock_t lock;
	CHECK_ZERO(mypthread_rwlock_init(&lock));
	for(int i=0; i<thread_num; i++) {
		tds[i].num=i;
		tds[i].lock=&lock;
		tds[i].max_sleep=max_sleep;
		tds[i].loops=loops;
		tds[i].reader=(i%2==0);
		CPU_ZERO(cpu_sets+i);
		CPU_SET(i%cpu_num, cpu_sets+i);
		CHECK_ZERO_ERRNO(pthread_attr_init(attrs+i));
		CHECK_ZERO_ERRNO(pthread_attr_setaffinity_np(attrs+i, sizeof(cpu_set_t), cpu_sets+i));
		CHECK_ZERO_ERRNO(pthread_create(threads+i, attrs+i, worker, tds+i));
	}
	for(int i=0; i<thread_num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], NULL));
	}
	CHECK_ZERO(mypthread_rwlock_destroy(&lock));
	delete[] threads;
	delete[] attrs;
	delete[] tds;
	delete[] cpu_sets;
	return EXIT_SUCCESS;
}
