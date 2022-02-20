/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <pthread.h>	// for pthread_t, pthread_create(3), pthread_join(3)
#include <stdio.h>	// for fprintf(3), stderr
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, atoi(3)
#include <CircularPipe.hh>	// for CircularPipe:Object
#include <err_utils.h>	// for CHECK_ZERO_ERRNO()
#include <unistd.h>	// for getpagesize(2)
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)

/*
 * This example shows how to implement a simple copy file using two threads.
 * This is useful if the two files are on different hard drives and have different
 * read/write bandwidths.
 *
 * References:
 * http://nandal.in/2012/04/copy-file-using-c-threads/
 *
 * EXTRA_LINK_FLAGS=-lpthread
 *
 * TODO:
 * - this example is not yet finished and has correctness issues. Fix them.
 */

typedef struct _thread_data {
	CircularPipe* cp;
	const char* filein;
	const char* fileout;
	bool eof;
} thread_data;

static void* reader(void* data) {
	thread_data* td=(thread_data*)data;
	int fd=CHECK_NOT_M1(open(td->filein, O_RDONLY));
	CircularPipe* cp=td->cp;
	do {
		while(cp->room() && !td->eof) {
			td->eof=cp->push(fd);
		}
		if(!td->eof) {
			// sleep on condition
		}
	} while(!td->eof);
	return NULL;
}

static void* writer(void* data) {
	thread_data* td=(thread_data*)data;
	int fd=CHECK_NOT_M1(open(td->filein, O_WRONLY));
	CircularPipe* cp=td->cp;
	do {
		while(cp->data()) {
			cp->pull(fd);
		}
		if(!td->eof) {
			// sleep on condition
		}
	} while(!td->eof && cp->data());
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	if(argc!=4) {
		fprintf(stderr, "%s: usage: %s [infile] [outfile] [numpages]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	const char* filein=argv[1];
	const char* fileout=argv[2];
	const unsigned int numpages=atoi(argv[3]);

	thread_data td;
	td.cp=new CircularPipe(numpages*getpagesize());
	td.filein=filein;
	td.fileout=fileout;
	td.eof=false;
	pthread_t pt_reader, pt_writer;
	CHECK_ZERO_ERRNO(pthread_create(&pt_reader, NULL, reader, &td));
	CHECK_ZERO_ERRNO(pthread_create(&pt_writer, NULL, writer, &td));
	CHECK_ZERO_ERRNO(pthread_join(pt_reader, NULL));
	CHECK_ZERO_ERRNO(pthread_join(pt_writer, NULL));
	return EXIT_SUCCESS;
}
