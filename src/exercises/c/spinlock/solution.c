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
#include <stdio.h>	// for fprintf(3)
#include <pthread.h>	// for pthread_create(3), pthread_join(3), pthread_attr_init(3), pthread_attr_setaffinity_np(3)
#include <unistd.h>	// for sysconf(3), sleep(3)
#include <sched.h>	// for CPU_ZERO(3), CPU_SET(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_NOT_M1()
#include <atomic_utils.h>	// for atomic_full_barrier()

/*
 * This is a solution to the spin locks exercise.
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 * EXTRA_COMPILE_FLAGS=
 *
 * TODO:
 * - get more ideas from the glibc implementation of spin locks
 */

// this is the spin lock implementation (pthread "like")
typedef struct _mypthread_spinlock_t {
	int val;
} mypthread_spinlock_t;

int mypthread_spin_init(mypthread_spinlock_t* lock) {
	lock->val=0;
	return 0;
}
int mypthread_spin_destroy(mypthread_spinlock_t* lock __attribute__((unused))) {
	// do nothing
	return 0;
}
int mypthread_spin_lock(mypthread_spinlock_t* lock) {
	// lets spin!...
	while(!__sync_bool_compare_and_swap(&(lock->val), 0, 1)) {
		// on platforms where atomic ops are NOT compiler
		// barrier you need to add a compiler barrier on
		// lock->val right here inside the loop...

		// glibc does a busy wait loop here without CAS to ease the load
		// on the other CPUS...
	}
	return 0;
}
int mypthread_spin_unlock(mypthread_spinlock_t* lock) {
	// Think: why is this needed?
	atomic_full_barrier();
	// no need for atomic ops here since we are sure we have the lock
	// and there is no competition with any other core
	lock->val=0;
	// overkill...
	// __sync_bool_compare_and_swap(&(lock->val), 1, 0);
	return 0;
}

const int loops=3;
mypthread_spinlock_t lock;
int counter=0;

void *worker(void *p) {
	const int cpu_num=CHECK_NOT_M1(sysconf(_SC_NPROCESSORS_ONLN));
	int num=*(int *)p;
	fprintf(stderr, "starting thread %d\n", num);
	int success=0;
	while(success<loops) {
		CHECK_ZERO(mypthread_spin_lock(&lock));
		if(counter%cpu_num==num) {
			fprintf(stderr, "thread %d caught lock\n", num);
			CHECK_ZERO(sleep(1));
			counter++;
			success++;
			fprintf(stderr, "thread %d released lock\n", num);
		}
		CHECK_ZERO(mypthread_spin_unlock(&lock));
	}
	fprintf(stderr, "ending thread %d\n", num);
	return NULL;
}

int main() {
	const int cpu_num=CHECK_NOT_M1(sysconf(_SC_NPROCESSORS_ONLN));
	const int thread_num=cpu_num;
	pthread_t* threads=(pthread_t*)malloc(thread_num*sizeof(pthread_t));
	pthread_attr_t* attrs=(pthread_attr_t*)malloc(thread_num*sizeof(pthread_attr_t));
	int* ids=(int*)malloc(thread_num*sizeof(int));
	cpu_set_t* cpu_sets=(cpu_set_t*)malloc(thread_num*sizeof(cpu_set_t));

	fprintf(stderr, "main starting\n");
	CHECK_ZERO(mypthread_spin_init(&lock));
	for(int i=0; i<thread_num; i++) {
		ids[i]=i;
		CPU_ZERO(cpu_sets+i);
		CPU_SET(i%cpu_num, cpu_sets+i);
		CHECK_ZERO_ERRNO(pthread_attr_init(attrs+i));
		CHECK_ZERO_ERRNO(pthread_attr_setaffinity_np(attrs+i, sizeof(cpu_set_t), cpu_sets+i));
		CHECK_ZERO_ERRNO(pthread_create(threads+i, attrs+i, worker, ids+i));
	}
	fprintf(stderr, "main ended creating threads\n");
	for(int i=0; i<thread_num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], NULL));
	}
	CHECK_ZERO(mypthread_spin_destroy(&lock));
	free(threads);
	free(attrs);
	free(ids);
	free(cpu_sets);
	fprintf(stderr, "counter is %d\n", counter);
	fprintf(stderr, "main ended\n");
	return EXIT_SUCCESS;
}
