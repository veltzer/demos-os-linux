/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for stderr, fprintf(3)
#include <stdlib.h>	// for malloc(3), atoi(3), free(3), EXIT_SUCCESS, EXIT_FAILURE
#include <sys/types.h>	// for open(2), lseek(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <sched_utils.h>// for sched_run_priority(), SCHED_FIFO_HIGH_PRIORITY:const
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <unistd.h>	// for close(2), read(2), lseek(2)
#include <assert.h>	// for assert(3)
#include <measure.h>	// for measure, measure_init(), measure_start(), measure_end(), measure_print()

/*
 * This example explores the performance of a read operation.
 * The example receives a file name which should not be in the OS cache.
 * The example then reads the file measuring the first read and then
 * reads it again, measuring the second read.
 * The difference should be clear.
 *
 * TODO:
 * - why, after the first read, when you run the app again, does it takemore for the first read?
 * - take care of short reads
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */

char* filename;
int fd;
off_t filesize;

void* func(void*) {
	// startup
	void* buf=malloc(filesize);
	ssize_t read_bytes=0;
	measure m;
	// first read
	measure_init(&m, "first read", read_bytes);
	measure_start(&m);
	read_bytes=CHECK_NOT_M1(read(fd, buf, filesize));
	assert(read_bytes==filesize);
	measure_end(&m);
	measure_print(&m);
	// seek back
	CHECK_NOT_M1(lseek(fd, 0, SEEK_SET));
	// second read
	measure_init(&m, "second read", read_bytes);
	measure_start(&m);
	read_bytes=CHECK_NOT_M1(read(fd, buf, filesize));
	assert(read_bytes==filesize);
	measure_end(&m);
	measure_print(&m);
	// seek back
	CHECK_NOT_M1(lseek(fd, 0, SEEK_SET));
	// third read
	measure_init(&m, "third read", read_bytes);
	measure_start(&m);
	read_bytes=CHECK_NOT_M1(read(fd, buf, filesize));
	assert(read_bytes==filesize);
	measure_end(&m);
	measure_print(&m);
	// shutdown
	free(buf);
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [filename]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	filename=argv[1];
	// lets open the file
	fd=CHECK_NOT_M1(open(filename, O_RDONLY));
	// lets find out the size of the file
	struct stat buf;
	CHECK_NOT_M1(fstat(fd, &buf));
	filesize=buf.st_size;
	sched_run_priority(func, NULL, SCHED_FIFO_HIGH_PRIORITY, SCHED_FIFO);
	CHECK_NOT_M1(close(fd));
	return EXIT_SUCCESS;
}
