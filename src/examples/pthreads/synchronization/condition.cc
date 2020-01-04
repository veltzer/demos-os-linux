/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for printf(3)
#include <pthread.h>	// for pthread_mutex_t, pthread_cond_t, pthread_mutex_lock(3), pthread_mutex_unlock(3), pthread_cond_wait(3), pthread_create(3), pthread_join(3), pthread_t
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_ZERO_ERRNO()

/*
 * This is an example that shows how to use pthread conditions.
 * The idea is to synchronize two threads. One thread will do work when count is between two values
 * and the other will do work when count is in any other range.
 * The thread that will do work for any other range is also reponsible for ending the entire program
 * by signaling the other thread that all work is done.
 *
 * Remmember that in order to use these you must protect the entire area where you are checking the condition
 * using a mutex. And since conditions do not come with mutexes you must create one yourself.
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */

static pthread_mutex_t condition_mutex=PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t condition_cond=PTHREAD_COND_INITIALIZER;

static int count=0;
static const int COUNT_DONE=10;
static const int COUNT_RANGE_MIN=3;
static const int COUNT_RANGE_MAX=6;
static bool done=false;

static void* functionCount1(void*) {
	while(true) {
		CHECK_ZERO_ERRNO(pthread_mutex_lock(&condition_mutex));
		while((count>=COUNT_RANGE_MIN && count<=COUNT_RANGE_MAX) && !done) {
			CHECK_ZERO_ERRNO(pthread_cond_wait(&condition_cond, &condition_mutex));
		}
		if(!done) {
			count++;
			printf("Counter value functionCount1: %d\n", count);
			if(count==COUNT_DONE) {
				done=true;
				CHECK_ZERO_ERRNO(pthread_cond_signal(&condition_cond));
			} else {
				if(count==COUNT_RANGE_MIN) {
					CHECK_ZERO_ERRNO(pthread_cond_signal(&condition_cond));
				}
			}
		}
		CHECK_ZERO_ERRNO(pthread_mutex_unlock(&condition_mutex));
		if(done) {
			printf("functionCount1 exiting...\n");
			break;
		}
	}
	return NULL;
}

static void *functionCount2(void*) {
	while(true) {
		CHECK_ZERO_ERRNO(pthread_mutex_lock(&condition_mutex));
		while((count<COUNT_RANGE_MIN || count>COUNT_RANGE_MAX) && !done) {
			CHECK_ZERO_ERRNO(pthread_cond_wait(&condition_cond, &condition_mutex));
		}
		if(!done) {
			count++;
			printf("Counter value functionCount2: %d\n", count);
			if(count==COUNT_RANGE_MAX+1) {
				CHECK_ZERO_ERRNO(pthread_cond_signal(&condition_cond));
			}
		}
		CHECK_ZERO_ERRNO(pthread_mutex_unlock(&condition_mutex));
		if(done) {
			printf("functionCount2 exiting...\n");
			break;
		}
	}
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	pthread_t thread1, thread2;
	CHECK_ZERO_ERRNO(pthread_create(&thread1, NULL, &functionCount1, NULL));
	CHECK_ZERO_ERRNO(pthread_create(&thread2, NULL, &functionCount2, NULL));
	CHECK_ZERO_ERRNO(pthread_join(thread1, NULL));
	CHECK_ZERO_ERRNO(pthread_join(thread2, NULL));
	return EXIT_SUCCESS;
}
