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
#include <pthread.h>	// for pthread_create(3), pthread_join(3), pthread_spin_init(3), pthread_spin_destroy(3), pthread_spin_lock(3), pthread_spin_unlock(3), pthread_attr_init(3), pthread_attr_setaffinity_np(3)
#include <unistd.h>	// for sysconf(3), sleep(3)
#include <sched.h>	// for CPU_ZERO(3), CPU_SET(3)
// #define DO_DEBUG
#include <trace_utils.h>// for DEBUG(), INFO()
#include <err_utils.h>	// for CHECK_ZERO(), CHECK_ZERO_ERRNO(), CHECK_NOT_M1()

/*
 * This example uses spin lock as a way to schedule thread to do
 * work in a certain order.
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */

const unsigned int loops=3;
static pthread_spinlock_t lock;
static unsigned int counter=0;
const unsigned int cpu_num=CHECK_NOT_M1(sysconf(_SC_NPROCESSORS_ONLN));

void *worker(void *p) {
	unsigned int num=*(unsigned int *)p;
	DEBUG("starting thread %d", num);
	unsigned int success=0;
	while(success<loops) {
		CHECK_ZERO_ERRNO(pthread_spin_lock(&lock));
		if(counter%cpu_num==num) {
			INFO("thread %d caught lock", num);
			CHECK_ZERO(sleep(1));
			counter++;
			success++;
			DEBUG("thread %d released lock", num);
		}
		CHECK_ZERO_ERRNO(pthread_spin_unlock(&lock));
	}
	DEBUG("ending thread %d", num);
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	const unsigned int thread_num=cpu_num;
	pthread_t* threads=new pthread_t[thread_num];
	pthread_attr_t* attrs=new pthread_attr_t[thread_num];
	unsigned int* ids=new unsigned int[thread_num];
	cpu_set_t* cpu_sets=new cpu_set_t[thread_num];

	DEBUG("main starting");
	CHECK_ZERO_ERRNO(pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE));
	for(unsigned int i=0; i<thread_num; i++) {
		ids[i]=i;
		CPU_ZERO(cpu_sets+i);
		CPU_SET(i%cpu_num, cpu_sets+i);
		CHECK_ZERO_ERRNO(pthread_attr_init(attrs+i));
		CHECK_ZERO_ERRNO(pthread_attr_setaffinity_np(attrs+i, sizeof(cpu_set_t), cpu_sets+i));
		CHECK_ZERO_ERRNO(pthread_create(threads+i, attrs+i, worker, ids+i));
	}
	DEBUG("main ended creating threads");
	for(unsigned int i=0; i<thread_num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], NULL));
	}
	CHECK_ZERO_ERRNO(pthread_spin_destroy(&lock));
	delete threads;
	delete attrs;
	delete ids;
	delete cpu_sets;
	DEBUG("counter is %d", counter);
	DEBUG("main ended");
	return EXIT_SUCCESS;
}
