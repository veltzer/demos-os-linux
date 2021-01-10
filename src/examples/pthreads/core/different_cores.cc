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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <pthread.h>	// for pthread_create(3), pthread_join(3)
#include <unistd.h>	// for sysconf(3)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_NOT_M1()

/*
 * This example shows distribution of pthreads across cores.
 * EXTRA_LINK_FLAGS=-lpthread
 */

void *worker(void *p) {
	int num=*(int *)p;
	//TRACE("starting thread [%d]", num);
	int cpunum=CHECK_NOT_M1(sched_getcpu());
	printf("thread [%d] running on cpu [%d]\n", num, cpunum);
	//TRACE("ending thread [%d]", num);
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	const int cpu_num=CHECK_NOT_M1(sysconf(_SC_NPROCESSORS_ONLN));
	const int num=cpu_num;
	pthread_t threads[num];
	int ids[num];

	for(int i=0; i<num; i++) {
		ids[i]=i;
		CHECK_ZERO_ERRNO(pthread_create(threads + i, NULL, worker, ids + i));
	}
	for(int i=0; i<num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], NULL));
	}
	return EXIT_SUCCESS;
}
