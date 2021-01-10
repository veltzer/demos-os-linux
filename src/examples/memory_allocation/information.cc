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
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, atoi(3), malloc(3), free(3)
#include <malloc.h>	// for malloc_stats(3), malloc_info(3), mallinfo(3), mallinfo(struct)
#include <pthread.h>	// for pthread_t(type), pthread_create(3), pthread_join(3)
#include <stdio.h>	// for fprintf(3), stderr(o)
#include <alloca.h>	// for alloca(3)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO()

/*
 * This example shows the how to get information about the internals of malloc
 * in various ways.
 * we use:
 * - malloc_stats(3)
 * - malloc_info(3)
 * - mallinfo(3)
 * In order to make it interesting it first does some multi threading
 * work with some memory allocations to make the pools/arenas appear.
 *
 * Notes:
 * - you can tune glibc's malloc(3) behaviour via the MALLOC_ARENA_MAX
 * environment variable. see the references below.
 * - you can tune glibc's malloc(3) behaviour via mallopt(3).
 *
 * References:
 * https://devcenter.heroku.com/articles/tuning-glibc-memory-behavior
 *
 * TODO:
 * - the print_mallinfo() function defined here should be in a
 * new shared include file: malloc_utils.h.
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */

unsigned int num_threads;
unsigned int num_allocations;
unsigned int size_to_alloc;

void *worker(void *p) {
	// lets allocate and deallocate memory...
	// we use alloca(3) for the list of pointers so as not to apply
	// more pressure on the memory allocating subsystem...
	void** arr=(void**)alloca(sizeof(void*)*num_allocations);
	for(unsigned int i=0; i<num_allocations; i++) {
		arr[i]=malloc(size_to_alloc);
	}
	for(unsigned int i=0; i<num_allocations; i++) {
		free(arr[i]);
	}
	return NULL;
}

void print_mallinfo(struct mallinfo* pmallinfo) {
	fprintf(stderr, "arena is [%d]\n", pmallinfo->arena);
	fprintf(stderr, "ordblks is [%d]\n", pmallinfo->ordblks);
	fprintf(stderr, "smblks is [%d]\n", pmallinfo->smblks);
	fprintf(stderr, "hblks is [%d]\n", pmallinfo->hblks);
	fprintf(stderr, "hblkhd is [%d]\n", pmallinfo->hblkhd);
	fprintf(stderr, "usmblks is [%d]\n", pmallinfo->usmblks);
	fprintf(stderr, "fsmblks is [%d]\n", pmallinfo->fsmblks);
	fprintf(stderr, "uordblks is [%d]\n", pmallinfo->uordblks);
	fprintf(stderr, "fordblks is [%d]\n", pmallinfo->fordblks);
	fprintf(stderr, "keepcost is [%d]\n", pmallinfo->keepcost);
}

int main(int argc, char** argv, char** envp) {
	if(argc!=4) {
		fprintf(stderr, "%s: usage: %s [num_threads] [num_allocations] [size_to_alloc]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: %s 5 1024 1024\n", argv[0], argv[0]);
		//fprintf(stderr, "%s: example: %s 5 1024 131072\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	num_threads=atoi(argv[1]);
	num_allocations=atoi(argv[2]);
	size_to_alloc=atoi(argv[3]);
	pthread_t threads[num_threads];

	for(unsigned int i=0; i<num_threads; i++) {
		CHECK_ZERO_ERRNO(pthread_create(threads + i, NULL, worker, NULL));
	}
	for(unsigned int i=0; i<num_threads; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], NULL));
	}
	malloc_stats();
	malloc_info(0, stderr);
	struct mallinfo mymallinfo=mallinfo();
	print_mallinfo(&mymallinfo);
	return EXIT_SUCCESS;
}
