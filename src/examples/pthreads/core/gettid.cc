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
#include <pthread.h>	// for pthread_create(3), pthread_join(3)
#include <sys/types.h>	// for getpid(2)
#include <unistd.h>	// for getpid(2)
#include <trace_utils.h>// for TRACE()
#include <err_utils.h>	// for CHECK_ZERO_ERRNO()
#include <pthread_utils.h>	// for gettid()

/*
 * This demo explored gettid() and getpid() issues on linux
 * This demo shows that:
 * - for the main thread (the one going from the main function) the thread id
 * is the same as the process id.
 * - for other threads the process id is the same but the thread id is different.
 * - pthread_self() is different than all of the above (some opaque value).
 * - there is no wrapper function in libc for gettid. I use a wrapper
 * here that I defined myself in <us_helper.hh>. Check it out. You can either
 * use that or use some framework which gives you wrappers.
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

void report(void) {
	TRACE("gettid() returned %d", gettid());
	TRACE("getpid() returned %d", getpid());
	TRACE("pthread_self() returned %lu", pthread_self());
}

void* doit(void*) {
	report();
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	report();
	pthread_t t1, t2;
	CHECK_ZERO_ERRNO(pthread_create(&t1, NULL, doit, NULL));
	CHECK_ZERO_ERRNO(pthread_create(&t2, NULL, doit, NULL));
	CHECK_ZERO_ERRNO(pthread_join(t1, NULL));
	CHECK_ZERO_ERRNO(pthread_join(t2, NULL));
	return EXIT_SUCCESS;
}
