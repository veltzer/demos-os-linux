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
#include <stdio.h>	// for stderr, fprintf(3)
#include <unistd.h>	// for syscall(2), __NR_getpid, getpid(3)
#include <stdlib.h>	// for atoi(3), EXIT_SUCCESS, EXIT_FAILURE
#include <sys/syscall.h>// for syscall(2)
#include <sys/time.h>	// for gettimeofday(2), struct timeval
#include <measure.h>	// for measure, measure_init(), measure_start(), measure_end(), measure_print()
#include <sched_utils.h>// for sched_run_priority(), SCHED_FIFO_HIGH_PRIORITY:const

/*
 * This demo times how long it takes to call a syscall.
 * It is a simple loop surrounded by gettimeofday.
 * We do call this code in high priority to make sure we do not get context switched
 * which will cause our measurements to be off.
 *
 * How long is it?
 * About 1/3 or even 1/5 mic on a fast computer.
 * About 1.2 mic on my old laptop.
 * About 0.5 mic on my new laptop.
 * Probably more on ARM processors although I am not sure.
 *
 * Notes:
 * - notice that syscall(__NR_getpid) is a little faster than gettimeofday. Obviously - it doesn't need
 * to get the time in the kernel.
 * - notice that getpid() is MUCH faster than syscall(__NR_getpid) because it caches the pid in user
 * space. It does not get optimized out (checked the disassembly). It also states that it is cached in user space since some version of glibc (check out man getpid())
 * - we also measure the time of a function call in user space, which, as expected, is a lot faster than even simple system calls.
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */

typedef enum _measure_type {
	MEASURE_GETTIMEOFDAY,
	MEASURE_SYSCALL_GETPID,
	MEASURE_GETPID,
	MEASURE_GETPPID,
	MEASURE_EMPTYFUNC,
} measure_type;

typedef struct _measure_val_and_name {
	measure_type val;
	const char* name;
} measure_val_and_name;

#define entry(x) { x, # x }
static measure_val_and_name measure_tbl[]={
	entry(MEASURE_GETTIMEOFDAY),
	entry(MEASURE_SYSCALL_GETPID),
	entry(MEASURE_GETPID),
	entry(MEASURE_GETPPID),
	entry(MEASURE_EMPTYFUNC),
};

typedef struct _thread_data {
	unsigned int count;
	measure_type type;
	const char* name;
} thread_data;

void emptyfunc() {
}
void emptyfunc() __attribute__((noinline));

void* func(void* p) {
	thread_data* td=(thread_data*)p;
	unsigned int count=td->count;
	char buf[1024];
	snprintf(buf, sizeof(buf), "%s", td->name);
	measure m;
	// think! why count+1?
	measure_init(&m, buf, count+1);
	measure_start(&m);
	for(unsigned int i=0; i<count; i++) {
		switch(td->type) {
			case MEASURE_GETTIMEOFDAY:
				struct timeval t3;
				gettimeofday(&t3, NULL);
				break;
			case MEASURE_SYSCALL_GETPID:
				syscall(__NR_getpid);
				break;
			case MEASURE_GETPID:
				getpid();
				break;
			case MEASURE_GETPPID:
				getppid();
				break;
			case MEASURE_EMPTYFUNC:
				emptyfunc();
				break;
		}
	}
	measure_end(&m);
	measure_print(&m);
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	// command line parsing
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [count]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	// get data into variables
	thread_data td;
	td.count=atoi(argv[1]);
	// lets start measuring...
	for(unsigned int i=0;i<ARRAY_SIZEOF(measure_tbl); i++) {
		td.name=measure_tbl[i].name;
		td.type=measure_tbl[i].val;
		sched_run_priority(func, &td, SCHED_FIFO_HIGH_PRIORITY, SCHED_FIFO);
	}
	return EXIT_SUCCESS;
}
