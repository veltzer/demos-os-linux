/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

const int num=2;
pid_t pids[num];
pthread_t threads[num];
int ids[num];
void* rets[num];
// the barrier that all threads will wait on...
pthread_barrier_t barrier;

static void* worker(void* p) {
	int num=*(int *)p;
	// fill in my own thread id so that the other threads
	// may know about me (and so kill me).
	pids[num]=gettid();
	CHECK_ONEOFTWO(pthread_barrier_wait(&barrier), 0, PTHREAD_BARRIER_SERIAL_THREAD);
	if(num==0) {
		while(true) {
			pause();
		}
	}
	if(num==1) {
		CHECK_NOT_M1(kill(pids[0], SIGKILL));
	}
	return NULL;
}

int main() {
	CHECK_ZERO_ERRNO(pthread_barrier_init(&barrier, NULL, num));
	for(int i=0; i<num; i++) {
		ids[i]=i;
		CHECK_ZERO_ERRNO(pthread_create(threads + i, NULL, worker, ids + i));
	}
	for(int i=0; i < num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], rets + i));
	}
	int i=0;
	while(true) {
		printf("Main thread is alive\n");
		sleep(1);
		i++;
		if(i==5) {
			printf("releasing the other threads to run\n");
			CHECK_ZERO_ERRNO(pthread_barrier_destroy(&barrier));
		}
		// pause();
	}
	return EXIT_SUCCESS;
}
