/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <sys/types.h>	// for getpid(2), ftruncate(2)
#include <unistd.h>	// for getpid(2), sleep(3), nice(2), fork(2), ftruncate(2)
#include <sched.h>	// for sched_setaffinity(2), CPU_ZERO(3), CPU_SET(3)
#include <stdio.h>	// for printf(3), fflush(3), fprintf(3)
#include <sys/mman.h>	// for shm_open(3), shm_unlink(3)
#include <sys/stat.h>	// for shm_open(3), shm_unlink(3)
#include <fcntl.h>	// for shm_open(3), shm_unlink(3)
#include <stdlib.h>	// for EXIT_SUCCESS, atoi(3), EXIT_FAILURE
#include <us_helper.h>	// for TRACE()
#include <err_utils.h>	// for CHECK_ZERO(), CHECK_NOT_M1(), CHECK_NOT_VOIDP()

/*
 * This examples shows the effect of the nice scheduling system (SCHED_OTHER).
 *
 * It forks several versions of heavy work each with a different nice level
 * bound to a core of your choosing.
 * Then you can compare the work done by each of these.
 * These forked processes report progress to the "master" process using shared
 * memory and compiler barriers.
 *
 * needed for shm_* functions...
 * EXTRA_LINK_FLAGS=-lrt
 *
 * References:
 * man 2 nice
 */

void make_child(const int niceval, int* const prog, int core) {
	pid_t pid=CHECK_NOT_M1(fork());
	if(pid!=0) {	// parent
		return;
	}
	// set the nice value for myself
	CHECK_NOT_M1(nice(niceval));
	// bind the entire process to the core required
	cpu_set_t myset;
	CPU_ZERO(&myset);
	CPU_SET(core, &myset);
	// pid=0 in the next call means the calling process
	CHECK_NOT_M1(sched_setaffinity(0, sizeof(myset), &myset));
	float sum=0;
	for(unsigned int i=0; i<100000000; i++) {
		for(unsigned int j=0; j<10000000; j++) {
			sum+=j;
		}
		// this barrier is really necessary. remove it and you wont see
		// progress at all...
		// this barrier is too heavy...
		asm volatile ("" ::: "memory");
		// this one isn't...
		//asm volatile ("" : "=g" (*prog) ::);
		*prog=*prog+1;
	}
	printf("child done with sum %f\n", sum);
}

int main(int argc, char** argv, char** envp) {
	if(argc<2 || ((argc-1)%2)!=0) {
		fprintf(stderr, "%s: usage: %s [niceval core]...\n", argv[0], argv[0]);
		fprintf(stderr, "%s: forexample: %s 2 0 1 0\n", argv[0], argv[0]);
		fprintf(stderr, "%s: will run two processes nice values 2 and 1 on core 0\n", argv[0]);
		fprintf(stderr, "%s: or try: %s 0 0 1 0 2 0 3 0 4 0 5 0 6 0 7 0 8 0 9 0\n", argv[0], argv[0]);
		fprintf(stderr, "%s: to see the graph of cpu distribution\n", argv[0]);
		return EXIT_FAILURE;
	}
	const int map_size=getpagesize();
	const char* shm_name="shared_mem";
	// put this in shared memory
	// don't check the return address in case it's the first time we run this software
	shm_unlink(shm_name);
	int smfd=CHECK_NOT_M1(shm_open(shm_name, O_CREAT|O_RDWR, 0));
	// we have to set the size otherwise it will not work
	CHECK_NOT_M1(ftruncate(smfd, map_size));
	void* ptr=CHECK_NOT_VOIDP(mmap(
			NULL,	/* addr: dont recommend address */
			map_size,	/* size: the size of the mapping */
			PROT_READ|PROT_WRITE,	/* prot: we just want read */
			MAP_SHARED,	/* flags: PRIVATE or SHARED ** MUST** be specified */
			smfd,	/* fd: our file descriptor */
			0	/* offset: from the begining of the file */
			), MAP_FAILED);
	int* iptr=(int*)ptr;
	// lets fork...
	const int num_processes=(argc-1)/2;
	int process_num=0;
	for(int i=1; i<argc; i++) {
		int* prog=iptr+process_num;
		*prog=0;
		int niceval=atoi(argv[i]);
		i++;
		int cpu=atoi(argv[i]);
		make_child(niceval, prog, cpu);
		process_num++;
	}
	int loop=0;
	while(true) {
		printf("\riteration %d: progress: ", loop);
		for(int i=0; i<num_processes; i++) {
			// compiler barrier to make it read the values pointed at by the pointers
			// again... This seems to work without the barrier but still...
			asm volatile ("" ::: "memory");
			int val=*(iptr+i);
			if(i<num_processes-1) {
				printf("%d, ", val);
			} else {
				printf("%d", val);
			}
		}
		// printf("\r");
		fflush(stdout);
		CHECK_ZERO(sleep(1));
		loop++;
	}
	CHECK_NOT_M1(munmap(ptr, map_size));
	CHECK_NOT_M1(shm_unlink(shm_name));
	return EXIT_SUCCESS;
}
