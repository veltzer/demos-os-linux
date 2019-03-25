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
#include <unistd.h>	// for sleep(3)
#include <pthread.h>	// for pthread_mutex_lock(3), pthread_mutex_unlock(3), PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_ASSERT()
#include <pthread_utils.h>	// for pthread_mutex_get_counter()

/*
 * This example shows that a recursive mutex can be locked by the same thread
 * as much as you want. But number of unlocks should be equal to the number
 * of locks. It also shows how to extract from the lock the number of times
 * that it was locked. This feature has no official API.
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */

static pthread_mutex_t mylock=PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

int main(int argc, char** argv, char** envp) {
	CHECK_ASSERT(pthread_mutex_get_counter(&mylock)==0);
	CHECK_ZERO_ERRNO(pthread_mutex_lock(&mylock));
	CHECK_ASSERT(pthread_mutex_get_counter(&mylock)==1);
	CHECK_ZERO_ERRNO(pthread_mutex_lock(&mylock));
	CHECK_ASSERT(pthread_mutex_get_counter(&mylock)==2);
	CHECK_ZERO_ERRNO(pthread_mutex_unlock(&mylock));
	CHECK_ASSERT(pthread_mutex_get_counter(&mylock)==1);
	CHECK_ZERO_ERRNO(pthread_mutex_unlock(&mylock));
	CHECK_ASSERT(pthread_mutex_get_counter(&mylock)==0);
	return EXIT_SUCCESS;
}
