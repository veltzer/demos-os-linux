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
#include <pthread.h>	// for pthread_t:struct, pthread_create(3), pthread_join(3)
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, rand(3), srand(3), rand_r(3), srandom(3), random(3), initstate_r(3), random_r(3)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_NOT_M1()
#include <sys/types.h>	// for getpid(2)
#include <unistd.h>	// for getpid(2)
#include <atomic>	// for std::atomic
#include <pthread_utils.h>	// for gettid()

/*
 * This example explores the performance of creating random numbers in a
 * multi-threaded context.
 * This example spawns one thread per code and tries to allocate as many random
 * numbers as it can in predefined time frames using various methods:
 * rand(3)
 * rand_r(3)
 * random(3)
 * random_r(3)
 *
 * Conclusions:
 * the re-entrant versions (_r) are 10 times better on my machine than the non
 * ones which, when run for long periods, show spinlocks taken in the kernel.
 * In addition random_r(3) is better at actually creating the random numbers than
 * then old rand_r(3) and is faster so it's the best one.
 *
 * References:
 * http://www.ellipsix.net/blog/2009/3/how-to-use-random-r.html
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 * for the atomics
 * EXTRA_COMPILE_FLAGS=-std=c++11
 *
 * TODO:
 * - add drand48(3) functions to the mix.
 */

static volatile bool stop_rand=false;
static volatile bool stop_rand_r=false;
static volatile bool stop_random=false;
static volatile bool stop_random_r=false;
std::atomic<int> counter_rand;
std::atomic<int> counter_rand_r;
std::atomic<int> counter_random;
std::atomic<int> counter_random_r;

static void *worker(void *) {
	while(!stop_rand) {
		rand();
		counter_rand++;
	}
	unsigned int seed=gettid();
	while(!stop_rand_r) {
		rand_r(&seed);
		counter_rand_r++;
	}
	while(!stop_random) {
		random();
		counter_random++;
	}
	const unsigned int state_len=32;
	char state[state_len];
	struct random_data data;
	CHECK_NOT_M1(initstate_r(gettid(), state, state_len, &data));
	while(!stop_random_r) {
		int32_t result;
		CHECK_NOT_M1(random_r(&data, &result));
		counter_random_r++;
	}
	return NULL;
}

int main() {
	// no errors from either getpid(2), srand(3), srandom(3)
	srand(getpid());
	srandom(getpid());
	counter_rand=0;
	counter_rand_r=0;
	counter_random=0;
	counter_random_r=0;
	const int cpu_num=CHECK_NOT_M1(sysconf(_SC_NPROCESSORS_ONLN));
	const int thread_num=cpu_num;
	pthread_t* threads=new pthread_t[thread_num];
	pthread_attr_t* attrs=new pthread_attr_t[thread_num];
	cpu_set_t* cpu_sets=new cpu_set_t[thread_num];
	int* ids=new int[thread_num];
	void** rets=new void*[thread_num];
	for(int i=0; i<thread_num; i++) {
		ids[i]=i;
		CPU_ZERO(cpu_sets+i);
		CPU_SET(i%cpu_num, cpu_sets+i);
		CHECK_ZERO_ERRNO(pthread_attr_init(attrs+i));
		CHECK_ZERO_ERRNO(pthread_attr_setaffinity_np(attrs+i, sizeof(cpu_set_t), cpu_sets+i));
		CHECK_ZERO_ERRNO(pthread_create(threads+i, attrs+i, worker, ids+i));
	}
	const unsigned int seconds_for_each=5;
	CHECK_ZERO(sleep(seconds_for_each));
	stop_rand=true;
	CHECK_ZERO(sleep(seconds_for_each));
	stop_rand_r=true;
	CHECK_ZERO(sleep(seconds_for_each));
	stop_random=true;
	CHECK_ZERO(sleep(seconds_for_each));
	stop_random_r=true;
	for(int i=0; i<thread_num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], &rets[i]));
	}
	printf("rand() results are %d\n", (int)counter_rand);
	printf("rand_r() results are %d\n", (int)counter_rand_r);
	printf("random() results are %d\n", (int)counter_random);
	printf("random_r() results are %d\n", (int)counter_random_r);
	delete[] threads;
	delete[] attrs;
	delete[] cpu_sets;
	delete[] rets;
	return EXIT_SUCCESS;
}
