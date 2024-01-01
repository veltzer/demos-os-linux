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
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>	// for EXIT_SUCCESS
#include <unistd.h>	// for sysconf(3)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_NOT_M1()

/*
 * This shows how to create threads with a certain affinity
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

void *worker(void *p) {
	int num=*(int *)p;
	fprintf(stderr, "starting thread %d\n", num);
	fprintf(stderr, "ending thread %d\n", num);
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	const int cpu_num=CHECK_NOT_M1(sysconf(_SC_NPROCESSORS_ONLN));
	const int num=10;
	pthread_t threads[num];
	pthread_attr_t attrs[num];
	cpu_set_t cpu_sets[num];
	int ids[num];
	void* rets[num];

	TRACE("started");
	for(int i=0; i<num; i++) {
		ids[i]=i;
		CPU_ZERO(cpu_sets + i);
		CPU_SET(i % cpu_num, cpu_sets + i);
		CHECK_ZERO_ERRNO(pthread_attr_init(attrs + i));
		CHECK_ZERO_ERRNO(pthread_attr_setaffinity_np(attrs + i, sizeof(cpu_set_t), cpu_sets + i));
		CHECK_ZERO_ERRNO(pthread_create(threads + i, attrs + i, worker, ids + i));
	}
	TRACE("created threads");
	for(int i=0; i<num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], rets + i));
	}
	TRACE("end");
	return EXIT_SUCCESS;
}
