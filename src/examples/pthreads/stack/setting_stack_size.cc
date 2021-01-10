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
#include <stdio.h>	// for fprintf(3), stderr
#include <stdlib.h>	// for EXIT_SUCCESS
#include <pthread.h>	// for pthread_attr_t:struct, pthread_t:struct, pthread_attr_init(3), pthread_attr_setstacksize(3), pthread_create(3), pthread_attr_destroy(3), pthread_join(3)
#include <unistd.h>	// for sleep(3)
#include <strings.h>	// for bzero(3)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_ZERO()

/*
 * This is an example of setting thread stack sizes
 * If you want to see the minor page faults happening while this
 * example is running run:
 * while [[ true ]]; do ps -C sizes.exe -o comm,min_flt,rss; sleep 1;done
 *
 * EXTRA_LINK_FLAGS=-lpthread
 *
 * TODO:
 * - show number of page faults for this application programmatically.
 * - First run a thread that allocates it's space in advance.
 * - Then run a thread that does not.
 * - show the info from /proc
 */

void ensure_space(unsigned int size) {
	// two ways of making sure there is enough space on the stack...
	int dummy;
	bzero(&dummy-size/2, size/2);
	// char buf[size];
	// bzero(buf,size);
}

void *worker(void *p) {
	int num=*(int *)p;
	ensure_space((num+1)*100*1024);
	fprintf(stderr, "starting thread %d\n", num);
	CHECK_ZERO(sleep(100));
	fprintf(stderr, "ending thread %d\n", num);
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	const int num=10;
	pthread_t threads[num];
	pthread_attr_t attrs[num];
	int ids[num];
	void* rets[num];

	fprintf(stderr, "main starting\n");
	for(int i=0; i<num; i++) {
		ids[i]=i;
		CHECK_ZERO_ERRNO(pthread_attr_init(attrs+i));
		CHECK_ZERO_ERRNO(pthread_attr_setstacksize(attrs+i, (i+1)*1024*1024));
		CHECK_ZERO_ERRNO(pthread_create(threads + i, attrs+i, worker, ids + i));
		CHECK_ZERO_ERRNO(pthread_attr_destroy(attrs+i));
		CHECK_ZERO(sleep(5));
	}
	fprintf(stderr, "main ended creating threads\n");
	for(int i=0; i<num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], rets + i));
	}
	fprintf(stderr, "main ended\n");
	return EXIT_SUCCESS;
}
