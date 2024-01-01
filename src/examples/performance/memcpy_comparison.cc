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
#include <stdio.h>	// for fprintf(3), stderr
#include <stdlib.h>	// for malloc(3), rand(3), EXIT_SUCCESS, EXIT_FAILURE, atoi(3)
#include <string.h>	// for malloc(3)
#include <measure.h>	// for measure, measure_init(), measure_start(), measure_end(), measure_print()
#include <sched_utils.h>// for SCHED_FIFO_HIGH_PRIORITY:const, sched_run_priority()

/*
 * This example compares memcpy(3) to copy by loop...
 * As you can see memcpy(3) is faster since it uses a specialized machine instruction to do the copying.
 * Actually glibc has an implementation of memcpy(3) and other
 * functions adjusted for each cpu and points the
 * memcpy(3) function pointer to the right implementation at initialization time.
 * See the disassembly for more details.
 * If you uncomment the rand() code you will find that memcpy(3) does not give you any slack: it does NOT
 * check the arguments passed to it for sanity. This is part of the philosophy of the GNU/Linux system.
 * The idea is that systems programmers can take care of themselves and the APIs should be as fast
 * as possible to cater for good programmers and not to aid the incompetant few with their debugging problems.
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

typedef struct _threaddata {
	unsigned int loop;
	size_t size;
	void* buf1;
	void* buf2;
} threaddata;

void* test_memcpy(void* p) {
	threaddata* td=(threaddata*)p;
	void* buf1=td->buf1;
	const void* buf2=td->buf2;
	size_t size=td->size;
	unsigned int loop=td->loop;
	measure m;
	measure_init(&m, "real memcpy", loop);
	measure_start(&m);
	for(unsigned int i=0; i<loop; i++) {
		memcpy(buf1, buf2, size);
	}
	measure_end(&m);
	measure_print(&m);
	return NULL;
}

void* test_char(void* p) {
	threaddata* td=(threaddata*)p;
	void* buf1=td->buf1;
	const void* buf2=td->buf2;
	size_t size=td->size;
	unsigned int loop=td->loop;
	char* bbuf1=(char*)buf1;
	const char* bbuf2=(const char*)buf2;
	measure m;
	measure_init(&m, "char by char", loop);
	measure_start(&m);
	for(unsigned int i=0; i < loop; i++) {
		for(unsigned int j=0; j<size; j++) {
			bbuf1[j]=bbuf2[j];
		}
	}
	measure_end(&m);
	measure_print(&m);
	return NULL;
}

void* test_imp1(void* p) {
	threaddata* td=(threaddata*)p;
	void* buf1=td->buf1;
	const void* buf2=td->buf2;
	size_t size=td->size;
	unsigned int loop=td->loop;
	measure m;
	measure_init(&m, "int by int (implementation I)", loop);
	measure_start(&m);
	for(unsigned int i=0; i < loop; i++) {
		for(unsigned int j=0; j<size/sizeof(int); j++) {
			((int*)buf1)[j]=((int*)buf2)[j];
		}
	}
	measure_end(&m);
	measure_print(&m);
	return NULL;
}

void* test_imp2(void* p) {
	threaddata* td=(threaddata*)p;
	void* buf1=td->buf1;
	const void* buf2=td->buf2;
	size_t size=td->size;
	unsigned int loop=td->loop;
	measure m;
	measure_init(&m, "int by int (implementation II)", loop);
	measure_start(&m);
	for(unsigned int i=0; i<loop; i++) {
		int* pbuf1=(int*)buf1;
		int* pbuf2=(int*)buf2;
		for(unsigned int j=0; j<size/sizeof(int); j++) {
			*pbuf1=*pbuf2;
			pbuf1++;
			pbuf2++;
		}
	}
	measure_end(&m);
	measure_print(&m);
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	if(argc!=3) {
		fprintf(stderr, "%s: usage: %s [loop] [size]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example is 10000 50000\n", argv[0]);
		return EXIT_FAILURE;
	}
	// parameters
	threaddata data;
	data.loop=atoi(argv[1]);
	data.size=atoi(argv[2]);
	data.buf1=malloc(data.size);
	data.buf2=malloc(data.size);

	/*
	 * if(rand()<RAND_MAX) {
	 * buf1=NULL;
	 * }
	 */
	sched_run_priority(test_memcpy, &data, SCHED_FIFO_HIGH_PRIORITY, SCHED_FIFO);
	sched_run_priority(test_char, &data, SCHED_FIFO_HIGH_PRIORITY, SCHED_FIFO);
	sched_run_priority(test_imp1, &data, SCHED_FIFO_HIGH_PRIORITY, SCHED_FIFO);
	sched_run_priority(test_imp2, &data, SCHED_FIFO_HIGH_PRIORITY, SCHED_FIFO);
	return EXIT_SUCCESS;
}
