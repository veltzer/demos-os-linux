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
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, rand(3)
#include <pthread.h>	// for pthread_t, pthread_attr_t, pthread_barrier_t, pthread_create(3)
#include <unistd.h>	// for sysconf(3)
#include <sched.h>	// for cpu_set_t, CPU_ZERO(3), CPU_SET(3), sched_getcpu(3)
#include <trace_utils.h>// for INFO()
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_ONEOFTWO(), CHECK_NOT_M1()
#include <us_helper.h>	// for no_params()
#include <lowlevel_utils.h>	// for mb(), fullmb()

/*
 * This example explores write/write memory reordering issues by the machine.
 * This example doesn't work because it tried to catch write/write
 * memory reorderings which are either don't existent on an x86 CPU.
 * Look at the article below and see exactly which memory reorderings
 * are performed by each CPU family.
 *
 * References:
 * http://en.wikipedia.org/wiki/Memory_ordering
 * http://ridiculousfish.com/blog/posts/barrier.html
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

typedef struct _shared_data {
	unsigned int attempts;
	unsigned int small;
	char buffer1[64];
	unsigned int large;
	char buffer2[64];
	bool running;
} shared_data;

typedef struct _thread_data {
	int num;
	shared_data* shared;
} thread_data;

static char buffer[100000];
void do_some_random_access() {
	unsigned int work=rand()%10000;
	for(unsigned int i=0; i<work; i++) {
		buffer[rand()%sizeof(buffer)]=rand()%256;
	}
}

static void *worker(void *p) {
	thread_data* td=(thread_data*)p;
	shared_data* sd=td->shared;
	if(td->num==0) {
		unsigned long attempts=0;
		volatile unsigned int large=sd->large;
		volatile unsigned int small=sd->small;
		for(unsigned int i=0; i<sd->attempts; i++) {
			large++;
			small++;
			sd->large=large;
			mb(sd->large);
			mb(sd->small);
			//asm volatile ("sfence" ::: "memory");
			//__sync_synchronize();
			//fullmb();
			sd->small=small;
			attempts++;
		}
		INFO("writer thread %d ended, attempts=%ld", td->num, attempts);
		sd->running=false;
		mb(sd->running);
	} else {
		unsigned long g_errors=0;
		unsigned long e_errors=0;
		unsigned long attempts=0;
		while(sd->running) {
			unsigned int small=sd->small;
			// force order between reading small and large, otherwise we may read large first
			// then wait a bit and then read small which could create errors
			mb(sd->small);
			mb(sd->large);
			unsigned int large=sd->large;
			if(small>large) {
				g_errors++;
			}
			if(small==large) {
				e_errors++;
			}
			attempts++;
			mb(sd->running);
		}
		INFO("end thread %d, attempts=%ld, g_errors=%ld, e_errors=%ld", td->num, attempts, g_errors, e_errors);
	}
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	no_params(argc, argv);
	const int thread_num=CHECK_NOT_M1(sysconf(_SC_NPROCESSORS_ONLN));
	const int cpu_num=CHECK_NOT_M1(sysconf(_SC_NPROCESSORS_ONLN));
	pthread_t* threads=new pthread_t[thread_num];
	pthread_attr_t* attrs=new pthread_attr_t[thread_num];
	thread_data* data=new thread_data[thread_num];
	cpu_set_t* cpu_sets=new cpu_set_t[thread_num];
	void** rets=new void*[thread_num];
	shared_data sd;
	sd.small=0;
	sd.large=1;
	sd.attempts=100000000;
	sd.running=true;

	INFO("start creating threads");
	for(int i=0; i<thread_num; i++) {
		data[i].num=i;
		data[i].shared=&sd;
		CPU_ZERO(cpu_sets+i);
		CPU_SET(i%cpu_num, cpu_sets+i);
		CHECK_ZERO_ERRNO(pthread_attr_init(attrs + i));
		CHECK_ZERO_ERRNO(pthread_attr_setaffinity_np(attrs + i, sizeof(cpu_set_t), cpu_sets + i));
		CHECK_ZERO_ERRNO(pthread_create(threads + i, attrs + i, worker, data + i));
	}
	INFO("created threads");
	INFO("joining threads");
	for(int i=0; i<thread_num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], rets+i));
	}
	INFO("joined threads");
	return EXIT_SUCCESS;
}
