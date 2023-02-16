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
#include <pthread.h>	// for pthread_key_t, pthread_key_create(3), pthread_setspecific(3), pthread_getspecific(3), pthread_create(3), pthread_join(3)
#include <stdlib.h>	// for EXIT_SUCCESS, malloc(3), free(3)
#include <trace_utils.h>// for TRACE()
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ZERO_ERRNO(), CHECK_NOT_NULL()

/*
 * This is a demo of thread local storage using the pthread API.
 *
 * Notes:
 * - notes that all threads *except* the main thread are using the TLS api to store
 * and retrieve that. This is because the main thread does not call pthread_exit(3)
 * but rather exit(3) and *does not therefore destroy it's own TLS allocated variables*
 * This is why, as in this example, it is customary for the main thread to avoid doing
 * any interesting code but rather just collect dead threads via pthread_join(3).
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

pthread_key_t key_myid;

static void* worker(void* arg) {
	CHECK_ZERO_ERRNO(pthread_setspecific(key_myid, arg));
	// now lets pull our id
	int myid=*(int*)CHECK_NOT_NULL(pthread_getspecific(key_myid));
	TRACE("myid is [%d]", myid);
	return NULL;
}

static void id_dealloc(void* ptr) {
	TRACE("deallocated [%p]", ptr);
	int* p=(int*)ptr;
	free(p);
}

static void run_threads() {
	const unsigned int num=4;
	pthread_t threads[num];
	for(unsigned int i=0; i<num; i++) {
		int* p=(int*)malloc(sizeof(int));
		*p=i;
		TRACE("allocated [%p]", p);
		CHECK_ZERO_ERRNO(pthread_create(threads + i, NULL, worker, p));
	}
	for(unsigned int i=0; i<num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], NULL));
	}
}

int main(int argc, char** argv, char** envp) {
	CHECK_ZERO_ERRNO(pthread_key_create(&key_myid, id_dealloc));
	run_threads();
	CHECK_ZERO_ERRNO(pthread_key_delete(key_myid));
	return EXIT_SUCCESS;
}
