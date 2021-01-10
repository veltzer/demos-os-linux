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
#include <pthread.h>	// for pthread_t, pthread_create(3), pthread_join(3), pthread_self(3)
#include <sys/types.h>	// for kill(2), SIGKILL
#include <signal.h>	// for kill(2), SIGKILL
#include <pthread_utils.h>	// for gettid(2)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_ONEOFTWO(), CHECK_NOT_M1()

/*
 * This example shows what happens when you kill a thread.
 * We spawn two threads and one kills the other.
 *
 * RESULTS:
 * - killing the thread causes the entire application to die (as expected).
 * this includes the main thread.
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */

const int num=2;
pid_t pids[num];
pthread_t threads[num];
int ids[num];
void* rets[num];

static void *worker(void *p) {
	int num=*(int *)p;
	if(num==0) {
		//CHECK_NOT_M1(prctl(PR_SET_NAME,"t1"));
		CHECK_ZERO_ERRNO(pthread_setname_np(pthread_self(),"t1"));
	}
	if(num==1) {
		//CHECK_NOT_M1(prctl(PR_SET_NAME,"t2"));
		CHECK_ZERO_ERRNO(pthread_setname_np(pthread_self(),"t2"));
	}
	while(1) {
		printf("thread %d is running...\n", num);
		sleep(1);
	}
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	for(int i=0; i<num; i++) {
		ids[i]=i;
		CHECK_ZERO_ERRNO(pthread_create(threads + i, NULL, worker, ids + i));
	}
	while(true) {
		printf("Main thread is alive\n");
		sleep(1);
	}
	for(int i=0; i < num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], rets + i));
	}
	return EXIT_SUCCESS;
}
