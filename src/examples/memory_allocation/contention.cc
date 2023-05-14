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
#include <pthread.h>	// for pthread_create(3), pthread_join(3)
#include <sched.h>	// for cpu_set_t, CPU_ZERO(3), CPU_SET(3)
#include <unistd.h>	// for sysconf(3)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_NOT_M1()
#include <stdlib.h>	// for EXIT_FAILURE, EXIT_SUCCESS, malloc(3), free(3)

/*
 * This example is designed to cause as much contention as possible
 * for the malloc(3) library and to investigate it's behaviour under
 * these conditions.
 *
 * trace this example using:
 * small allocations:
 * strace -f ./src/examples/memory_allocation/contention.elf 1000000 100 10 300 1
 * large (mmap) allocations:
 * strace -f -e trace=mmap,munmap ./src/examples/memory_allocation/contention.elf 1000000 100 1 4000 4096
 *
 * TODO:
 * - Add the possibility to do more than one allocation at a time (do them in batches).
 * - Analyze the results of this example.
 * - Show that we block sometimes and sometimes we dont.
 * - Show that we can block on futex in user space and in kernel space on the mmap(2) semaphore.
 * - Show that we can play around with the blocking using MALLOC_ARENA_MAX.
 * MALLOC_ARENA_MAX is quite large. 32-bit systems get twice the number of cores and 64-bit systems get 8 times the number of cores.
 *
 * References:
 * - https://devcenter.heroku.com/articles/tuning-glibc-memory-behavior
 * - https://www.infobright.com/index.php/malloc_arena_max/#.VfhyG7PtD0o
 * - http://journal.siddhesh.in/posts/malloc-per-thread-arenas-in-glibc.html
 * - http://www.gnu.org/software/libc/manual/html_node/Malloc-Tunable-Parameters.html
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

// data available to the threads
static int num_iterations;
static int num_allocations;
static int size_min;
static int size_max;
static int mul;

void *worker(void *p) {
	//int num=*(int *)p;
	int diff=size_max-size_min;
	for(int i=0;i<num_iterations;i++) {
		void** buffers=new void*[num_allocations];
		for(int j=0;j<num_allocations;j++) {
			int size_to_alloc;
			if(diff==0) {
				size_to_alloc=size_min*mul;
			} else {
				size_to_alloc=(size_min+rand()%diff)*mul;
			}
			buffers[j]=CHECK_NOT_NULL(malloc(size_to_alloc));
		}
		for(int j=0;j<num_allocations;j++) {
			// free(3) has not return value
			free(buffers[j]);
		}
		delete[] buffers;
	}
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	if(argc!=6) {
		fprintf(stderr, "argc is %d\n", argc);
		fprintf(stderr, "%s: usage: %s [num_iterations] [num_allocations] [size_min] [size_max] [mul]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: %s 1000000 100 1 100 4096\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	num_iterations=atoi(argv[1]);
	num_allocations=atoi(argv[2]);
	size_min=atoi(argv[3]);
	size_max=atoi(argv[4]);
	mul=atoi(argv[5]);

	const int cpu_num=CHECK_NOT_M1(sysconf(_SC_NPROCESSORS_ONLN));
	const int num_threads=cpu_num;

	pthread_t* threads=new pthread_t[num_threads];
	pthread_attr_t* attrs=new pthread_attr_t[num_threads];
	cpu_set_t* cpu_sets=new cpu_set_t[num_threads];
	int* ids=new int[num_threads];

	for(int i=0; i<num_threads; i++) {
		ids[i]=i;
		CPU_ZERO(cpu_sets + i);
		CPU_SET(i % cpu_num, cpu_sets + i);
		CHECK_ZERO_ERRNO(pthread_attr_init(attrs + i));
		CHECK_ZERO_ERRNO(pthread_attr_setaffinity_np(attrs + i, sizeof(cpu_set_t), cpu_sets + i));
		CHECK_ZERO_ERRNO(pthread_create(threads + i, attrs + i, worker, ids + i));
	}
	for(int i=0; i<num_threads; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], NULL));
	}
	delete[] threads;
	delete[] attrs;
	delete[] cpu_sets;
	delete[] ids;
	return EXIT_SUCCESS;
}
