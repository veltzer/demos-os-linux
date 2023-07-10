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
#include <pthread.h>	// for pthread_t, pthread_create(3), pthread_join(3), pthread_self(3)
#include <unistd.h>	// for sleep(3), getpid(2)
#include <sys/types.h>	// for getpid(2)
#include <sched.h>	// for sched_getcpu(2)
#include <trace_utils.h>// for TRACE()
#include <pthread_utils.h>	// for gettid(2), gettid_cached()
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_ZERO()

/*
 * This is a standard pthread demo.
 * This shows the various attributes of each of the threads:
 * pthread_self, tid, tid cached, pid, core it is running
 * on.
 *
 * EXTRA_COMPILE_FLAGS=-g
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

static void *worker(void *p) {
	int num=*(int *)p;
	TRACE("starting thread %d", num);
	pthread_t t=pthread_self();
	TRACE("thread %d: pthread_self is %lu", num, t);
	TRACE("thread %d: gettid() is %d", num, gettid());
	TRACE("thread %d: gettid_cached() is %d", num, gettid_cached());
	TRACE("thread %d: getpid() is %d", num, getpid());
	TRACE("thread %d: sched_getcpu() is %d", num, sched_getcpu());
	CHECK_ZERO(sleep(60));
	TRACE("ending thread %d", num);
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	const int num=10;
	pthread_t threads[num];
	int ids[num];
	void* rets[num];

	TRACE("main getpid() is %d", getpid());
	TRACE("main starting");
	for(int i=0; i<num; i++) {
		ids[i]=i;
		CHECK_ZERO_ERRNO(pthread_create(threads + i, NULL, worker, ids + i));
	}
	TRACE("main ended creating threads");
	for(int i=0; i < num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], rets + i));
	}
	TRACE("main ended");
	return EXIT_SUCCESS;
}
