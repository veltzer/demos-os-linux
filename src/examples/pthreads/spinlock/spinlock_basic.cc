/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2024 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <pthread.h>	// for pthread_spin_init(3), pthread_spin_lock(3), pthread_spin_unlock(3), pthread_spin_destroy(3), pthread_create(3), pthread_join(3)
#include <trace_utils.h>// for TRACE()
#include <err_utils.h>	// for CHECK_ZERO(), CHECK_ZERO_ERRNO()

/*
 * This is a demo for thread spin locks.
 * Run on any machine and you will see a problem in that they will block
 * each other. This blockage does not turn into a dead lock since the OS
 * will preempt them and eventually each will get it's turn.
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 *
 * TODO:
 * - get the thread number from the command line
 */

static pthread_spinlock_t mylock;

static void *worker(void*) {
	for(int i=0; i<10; i++) {
		// TRACE("before lock");
		CHECK_ZERO_ERRNO(pthread_spin_lock(&mylock));
		TRACE("sleeping");
		CHECK_ZERO(sleep(1));
		CHECK_ZERO_ERRNO(pthread_spin_unlock(&mylock));
		// TRACE("after lock");
		CHECK_ZERO(sleep(1));
	}
	return NULL;
}

int main() {
	TRACE("initializing the lock...");
	CHECK_ZERO_ERRNO(pthread_spin_init(&mylock, PTHREAD_PROCESS_PRIVATE));
	const unsigned int thread_num=2;
	pthread_t threads[thread_num];
	int ids[thread_num];
	TRACE("starting threads...");
	for(unsigned int i=0; i<thread_num; i++) {
		ids[i]=i;
		CHECK_ZERO_ERRNO(pthread_create(threads + i, NULL, worker, ids + i));
	}
	TRACE("finished creating threads, joining them...");
	for(unsigned int i=0; i<thread_num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], NULL));
	}
	TRACE("joined all threads, destroying the lock...");
	CHECK_ZERO_ERRNO(pthread_spin_destroy(&mylock));
	return EXIT_SUCCESS;
}
