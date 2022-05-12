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
#include <unistd.h>	// for sleep(3)
#include <pthread.h>	// for pthread_mutex_init(3), pthread_mutex_lock(3), pthread_mutex_unlock(3), pthread_mutex_destroy(3), pthread_create(3), pthread_join(3)
#include <trace_utils.h>// for TRACE()
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_ZERO()

/*
 * This example shows that locking and unlocking in the uncontended case DOES NOT
 * DO ANY SYSTEM CALL IN LINUX because the default implementation of a mutex in
 * Linux is really a futex(2).
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */

int main(int argc, char** argv, char** envp) {
	pthread_mutex_t mylock;
	CHECK_ZERO_ERRNO(pthread_mutex_init(&mylock, NULL));
	int counter=0;
	TRACE("before loop");
	while(counter<100) {
		CHECK_ZERO_ERRNO(pthread_mutex_lock(&mylock));
		CHECK_ZERO_ERRNO(pthread_mutex_unlock(&mylock));
		counter++;
	}
	TRACE("after loop");
	CHECK_ZERO_ERRNO(pthread_mutex_destroy(&mylock));
	return EXIT_SUCCESS;
}
