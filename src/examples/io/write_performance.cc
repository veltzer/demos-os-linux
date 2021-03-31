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
#include <stdio.h>	// for fprintf(3)
#include <stdlib.h>	// for malloc(3), atoi(3), free(3), EXIT_SUCCESS, EXIT_FAILURE
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <unistd.h>	// for close(2), write(2)
#include <lowlevel_utils.h>	// for getticks()
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <sched_utils.h>// sched_run_priority(), SCHED_FIFO_HIGH_PRIORITY:const
#include <Stat.hh>	// for Stat:Object

/*
 * This example explores the performance of the write system call...
 *
 * The example is shows that writes to /dev/null have
 * constant performance while writes to a file are different: when
 * the OS buffers are empty they are fast (memcpy to kernel) and when
 * they are full they block...
 *
 * example of running this could be:
 * ./src/examples/io/write_performance.exe /tmp/foo 2000000 100 100 10000000 0
 * You are supposed to see two peaks: one for fast writes which just copies to
 * kernel and one for slow writes that block you...
 *
 * You can also use iotop to see the process consuming first place in the io
 * category.
 *
 * EXTRA_LINK_FLAGS=-lcpufreq -lpthread
 */

char* filename;
unsigned int bufsize;
unsigned int count;
unsigned int binnumber;
double binsize;
double binmean;

void* func(void*) {
	void* buf=malloc(bufsize);
	Stat s(binnumber, binsize, binmean);
	int fd=CHECK_NOT_M1(open(filename, O_RDWR | O_CREAT, 0666));
	for(unsigned int i=0; i<count; i++) {
		ticks_t t1=getticks();
		CHECK_NOT_M1(write(fd, buf, bufsize));
		ticks_t t2=getticks();
		// ticks_t diff=t2-t1;
		unsigned int mic_diff=get_mic_diff(t1, t2);
		s.accept(mic_diff);
	}
	CHECK_NOT_M1(close(fd));
	s.print();
	free(buf);
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	if(argc!=7) {
		fprintf(stderr, "%s: usage: %s [filename] [bufsize] [count] [binnumber] [binsize] [binmean]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	filename=argv[1];
	bufsize=atoi(argv[2]);
	count=atoi(argv[3]);
	binnumber=atoi(argv[4]);
	binsize=atof(argv[5]);
	binmean=atof(argv[6]);
	sched_run_priority(func, NULL, SCHED_FIFO_HIGH_PRIORITY, SCHED_FIFO);
	return EXIT_SUCCESS;
}
