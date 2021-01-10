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
#include <pthread.h>	// for pthread_t, pthread_create(3), pthread_join(3), pthread_self(3)
#include <trace_utils.h>// for TRACE()
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_ZERO()
#include <work_utils.h>	// for do_work()

/*
 * This is a demo of a multi-threaded application which does some work.
 * Watch it using top(1) and the 'H' button which shows you threads
 * to see the thread actually doing the work.
 * Note that without the 'H' view top will actually show you the accumulated
 * CPU time that all threads in this process take which, on a multi-core
 * machine, may be greater than 100%.
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */
void *worker(void *p) {
	// int num=*(int *)p;
	// TRACE("starting thread %d", num);
	// pthread_t t=pthread_self();
	// TRACE("pthread_self is %lu", t);
	do_work(30);
	// TRACE("ending thread %d", num);
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	const int num=10;
	pthread_t threads[num];
	int ids[num];
	void* rets[num];
	// TRACE("main starting");
	for(int i=0; i<num; i++) {
		ids[i]=i;
		CHECK_ZERO_ERRNO(pthread_create(threads + i, NULL, worker, ids + i));
	}
	// TRACE("main ended creating threads");
	for(int i=0; i<num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], rets + i));
	}
	// TRACE("main ended");
	return EXIT_SUCCESS;
}
