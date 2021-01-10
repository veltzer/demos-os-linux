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
#include <pthread.h>	// for pthread_key_t, pthread_key_create(3), pthread_setspecific(3), pthread_getspecific(3)
#include <stdlib.h>	// for EXIT_SUCCESS, malloc(3), free(3)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_NOT_NULL(), CHECK_ZERO()
#include <pthread_utils.h>	// for pthread_main_np()
#include <trace_utils.h>	// for TRACE()

/*
 * This example shows one way to fix the fact that main thread does not release TLS entries.
 * The idea is to replace pthread_setspecific(3) with a different implementation which
 * arranges for deallocation via on_exit(3).
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */

static void delfunc(int exitstatus, void* ptr) {
	TRACE("releasing the pointer [%p]", ptr);
	free(ptr);
}

static int my_pthread_setspecific(pthread_key_t key, const void *value) {
	if(pthread_main_np()) {
		CHECK_ZERO(on_exit(delfunc, (void*)value));
	}
	return pthread_setspecific(key, value);
}

static pthread_key_t key_myid;

static void id_dealloc(void* ptr) {
	TRACE("releasing the pointer [%p]", ptr);
	free(ptr);
}

static void* worker(void* arg) {
	CHECK_ZERO_ERRNO(my_pthread_setspecific(key_myid, arg));
	// now lets pull our id
	int myid=*(int*)CHECK_NOT_NULL(pthread_getspecific(key_myid));
	TRACE("my id is %d", myid);
	return NULL;
}

static void run_threads() {
	const unsigned int num=4;
	pthread_t threads[num];
	for(unsigned int i=0; i<num; i++) {
		int* p=(int*)malloc(sizeof(int));
		*p=i;
		CHECK_ZERO_ERRNO(pthread_create(threads + i, NULL, worker, p));
	}
	for(unsigned int i=0; i<num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], NULL));
	}
}

int main(int argc, char** argv, char** envp) {
	CHECK_ZERO_ERRNO(pthread_key_create(&key_myid, id_dealloc));
	int* p=(int*)malloc(sizeof(int));
	*p=1000;
	worker(p);
	run_threads();
	CHECK_ZERO_ERRNO(pthread_key_delete(key_myid));
	return EXIT_SUCCESS;
}
