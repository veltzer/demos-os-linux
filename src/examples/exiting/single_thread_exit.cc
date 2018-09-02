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
#include <stdio.h>	// for printf(3)
#include <pthread.h>	// for pthread_t, pthread_create(3), pthread_join(3), pthread_self(3)
#include <unistd.h>	// for sleep(3), _exit(2)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_ZERO()
#include <unistd.h>	// for syscall(2)
#include <sys/syscall.h>// for SYS_exit:int, SYS_exit_group:int

/*
 * This example shows how you can exit just a single thread using the
 * _exit(2) system call.
 *
 * Notes:
 * - we do not use the glibc wrapper for _exit(2) since it calls the
 * exit_group(2) kernel sys call which will exit all threads. Instead
 * we call the _exit(2) system call directly...
 * - pthread_exit(2) is actually built on top of the real _exit(2)
 * system call.
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */

static void *worker(void *p) {
	int num=*(int *)p;
	printf("starting thread %d\n", num);
	for(int i=0; i<60; i++) {
		printf("%d: thread %d is alive...\n", i, num);
		CHECK_ZERO(sleep(1));
		if(i==10 && num%2==0) {
			syscall(SYS_exit, EXIT_SUCCESS);
		}
		// this will really end all threads (because it is really exit_group(2))
		if(i==15 && num==7) {
			// the next two are interchangeable...
			_exit(EXIT_SUCCESS);
			// syscall(SYS_exit_group, EXIT_SUCCESS);
		}
	}
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	const int num=10;
	pthread_t threads[num];
	int ids[num];
	void* rets[num];
	for(int i=0; i<num; i++) {
		ids[i]=i;
		CHECK_ZERO_ERRNO(pthread_create(threads + i, NULL, worker, ids + i));
	}
	for(int i=0; i<num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], rets + i));
	}
	return EXIT_SUCCESS;
}
