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
#include <pthread.h>	// for pthread_create(3), pthread_join(3), pthread_t
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_ZERO_ERRNO()

/*
 * This example shows how to exit threads (pthread_exit and return
 * from thread function).
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

void* PrintHello(void *threadid) {
	int *id_ptr, taskid;
	id_ptr=(int *) threadid;
	taskid=*id_ptr;
	printf("Thread %d says hello\n", taskid);
	return (void *)((long)taskid*2);
	// another option would be to call
	// pthread_exit((void *)taskid);
}

int main(int argc, char** argv, char** envp) {
	const int NUM_THREADS=8;
	pthread_t threads[NUM_THREADS];
	int t[NUM_THREADS];
	// init all values to be passed to threads
	for(int i=0; i<NUM_THREADS; i++) {
		t[i]=i;
	}
	// start all threads giving each it's input
	for(int i=0; i<NUM_THREADS; i++) {
		CHECK_ZERO_ERRNO(pthread_create(&threads[i], NULL, PrintHello, (void *)(t+i)));
	}
	// wait for all threads to finish, order does not matter
	for(int i=0; i<NUM_THREADS; i++) {
		void* retval;
		CHECK_ZERO_ERRNO(pthread_join(threads[i], &retval));
		printf("thread %d returned value %p\n", i, retval);
	}
	return EXIT_SUCCESS;
}
