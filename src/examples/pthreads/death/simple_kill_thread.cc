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
	// fill in my own thread id so that the other threads
	// may know about me (and so kill me).
	pids[num]=gettid();
	if(num==0) {
		while(true) {
			printf("thread 0 is alive...\n");
			sleep(1);
		}
	}
	if(num==1) {
		int i=0;
		while(true) {
			printf("thread 1 is alive...\n");
			sleep(1);
			i++;
			if(i==5) {
				//*(char*)0=0;
				//CHECK_ZERO_ERRNO(pthread_kill(threads[0], SIGKILL));
				CHECK_NOT_M1(tgkill(getpid(), pids[0], SIGKILL));
				//printf("killing my brother...\n");
				//CHECK_NOT_M1(kill(pids[0],SIGKILL));
			}
		}
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
