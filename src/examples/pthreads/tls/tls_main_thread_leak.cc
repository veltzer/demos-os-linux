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
#include <pthread.h>	// for pthread_key_t, pthread_key_create(3), pthread_setspecific(3), pthread_getspecific(3)
#include <stdlib.h>	// for EXIT_SUCCESS, malloc(3), abort(3)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_NOT_NULL()

/*
 * This is an example intended to show that the main thread in a multi threaded
 * pthread application does not have it's TLS variables destroyed when it exists.
 *
 * The reason for this is that the main thread in a pthread app exists using exit(3) and not
 * using pthread_exit(3). exit(3) does not call the pthread TLS destuctors. This means
 * that you main thread must take care to destroy it's own TLS data manually or via atexit(3)/on_exit(3) or
 * any other mechanism or avoid using TLS.
 *
 * This means that the main thread in pthreads is 'special' in a way. This is not the only feature
 * that makes it 'special' too. For instance, the main thread will call functions registered
 * via pthread_cleanup_push(3), pthread_cleanup_pop(3).
 *
 * The fact that the main thread is 'special' in several ways leads naturally to the design where
 * the main pthread thread should only do pthread_join(3) and nothing else interesting. This is
 * the recommended design.
 *
 * References:
 * http://stackoverflow.com/questions/6357154/destruction-order-of-the-main-thread-and-the-use-of-pthread-key-create
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */

static pthread_key_t key_myid;

static void id_dealloc(void* ptr) {
	abort();
}

int main(int argc, char** argv, char** envp) {
	CHECK_ZERO_ERRNO(pthread_key_create(&key_myid, id_dealloc));
	int* p=(int*)malloc(sizeof(int));
	*p=1000;
	CHECK_ZERO_ERRNO(pthread_setspecific(key_myid, p));
	CHECK_NOT_NULL(pthread_getspecific(key_myid));
	CHECK_ZERO_ERRNO(pthread_key_delete(key_myid));
	return EXIT_SUCCESS;
}
