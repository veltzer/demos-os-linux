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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for malloc(3)
#include <unistd.h>	// for sleep(3)
#include <sys/mman.h>	// for mlockall(2)
#include <string.h>	// for memset(3)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <time.h>	// for clock_gettime(2), CLOCK_PROCESS_CPUTIME_ID:const, CLOCK_REALTIME:const
#include <timespec_utils.h>	// for timespec_set(), timespec_add_secs(), timespec_sub(), timespec_snprintf()
#include <proc_utils.h>	// for getrusage_show_vmem()

/*
 * EXTRA_COMPILE_FLAGS_AFTER_DUMMY=-O0 -g3
 */

typedef enum {
	TYPE_MEMSET,
	TYPE_MEMSET1,
	TYPE_TOUCH,
	TYPE_MLOCKALL,
	TYPE_NOTSET,
} test_type;

//static int clock_type=CLOCK_PROCESS_CPUTIME_ID;
static int clock_type=CLOCK_REALTIME;

static test_type current_test_type=TYPE_NOTSET;

int main(int argc, char** argv, char** envp) {
	printf("starting\n");
	getrusage_show_vmem();
	current_test_type=(test_type)atoi(argv[1]);
	// lets allocate a buffer of 1GB
	const size_t size_to_alloc=(size_t)atoi(argv[2]);
	printf("size_to_alloc is %lu\n", size_to_alloc);
	const unsigned int number_of_pages_to_touch=100;
	//memset(buf, 1, size_to_alloc);
	//madvise(buf, size_to_alloc, MADV_RANDOM);
	printf("before\n");
	getrusage_show_vmem();
	struct timespec start;
	CHECK_NOT_M1(clock_gettime(clock_type, &start));

	char* buf=NULL;
	char* vbuf;
	size_t size;
	unsigned long sum;
	switch(current_test_type) {
		case TYPE_MEMSET:
			buf=(char*)CHECK_NOT_NULL(malloc(size_to_alloc));
			memset(buf, 0, size_to_alloc);
			/*
			sum=0;
			for(unsigned long i=0;i<100000000;i++) {
				sum+=i*i;
			}
			printf("sum is %lu\n", sum);
			*/
			break;
		case TYPE_MEMSET1:
			buf=(char*)CHECK_NOT_NULL(malloc(size_to_alloc));
			memset(buf, 1, size_to_alloc);
			break;
		case TYPE_TOUCH:
			buf=(char*)CHECK_NOT_NULL(malloc(size_to_alloc));
			vbuf=buf;
			size=size_to_alloc/4096;
			sum=0;
			while(size>0) {
				*vbuf=0;
				vbuf+=4096;
				size--;
			}
			printf("sum is %lu\n", sum);
			break;
		case TYPE_MLOCKALL:
			buf=(char*)CHECK_NOT_NULL(malloc(size_to_alloc));
			CHECK_NOT_M1(mlockall(MCL_CURRENT|MCL_FUTURE));
			break;
		case TYPE_NOTSET:
			fprintf(stderr, "Uh uh!\n");
			exit(EXIT_FAILURE);
			break;
	}

	struct timespec end;
	CHECK_NOT_M1(clock_gettime(clock_type, &end));
	printf("after\n");
	getrusage_show_vmem();
	struct timespec diff;
	timespec_sub(&diff, &end, &start);
	char print_buf[1024];
	timespec_snprintf(print_buf, sizeof(print_buf), &diff, 0);
	printf("diff: %s\n", print_buf);

	unsigned int counter=0;
	unsigned int pages=size_to_alloc/4096;
	while(true) {
		// touch some memory
		unsigned int pages_to_touch=min(pages, number_of_pages_to_touch);
		for(unsigned int i=0;i<pages_to_touch;i++) {
			*buf=0;
			buf+=4096;
		}
		pages-=pages_to_touch;
		counter++;
		printf("I touched some memory, %d\n", counter);
		// sleep
		sleep(1);
	}
	return EXIT_SUCCESS;
}
