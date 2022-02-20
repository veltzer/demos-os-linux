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
#include <stdio.h>	// for fprintf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <sys/ioctl.h>	// for ioctl(2)
#include <pthread.h>	// for pthread_create(3), pthread_join(3)
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ZERO_ERRNO()
#include <multiproc_utils.h>	// for my_system()
#include <lowlevel_utils.h>	// for getticks(), get_mic_diff()
#include "shared.h"	// for the ioctl codes

/*
 * This is a demo of how to put a thread to sleep and wake it up
 * from another thread... This is done via the complete function
 *
 * EXTRA_LINK_FLAGS=-lpthread -lcpufreq
 */

// file descriptor to be used all over
int fd;
// timeout for threads going to sleep
const int timeout=10000;

void *wait_function(void *p) {
	fprintf(stderr, "wait thread started\n");
	ticks_t t1=getticks();
	// wait thread going to sleep
	CHECK_NOT_M1(ioctl(fd, IOCTL_COMPLETE_WAIT_INTERRUPTIBLE_TIMEOUT, 10000));
	ticks_t t2=getticks();
	fprintf(stderr, "took %d micros\n", get_mic_diff(t1, t2));
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	// file to be used
	const char *filename="/dev/mod_complete";
	printf("Inserting the driver...\n");
	my_system("sudo rmmod mod_complete");
	my_system("sudo insmod ./mod_complete.ko");
	my_system("sudo chmod 666 %s", filename);

	// we are the in the parent of the threads - connect to the device
	fd=CHECK_NOT_M1(open(filename, O_RDWR));
	// initialize the completion
	CHECK_NOT_M1(ioctl(fd, IOCTL_COMPLETE_INIT, NULL));

	pthread_t thread_wait1, thread_wait2;
	CHECK_ZERO_ERRNO(pthread_create(&thread_wait1, NULL, wait_function, NULL));
	CHECK_ZERO_ERRNO(pthread_create(&thread_wait2, NULL, wait_function, NULL));
	waitkey("press any key to wake the thread up");
	// waking the thread
	CHECK_NOT_M1(ioctl(fd, IOCTL_COMPLETE_COMPLETE_ALL, NULL));
	CHECK_ZERO_ERRNO(pthread_join(thread_wait1, NULL));
	CHECK_ZERO_ERRNO(pthread_join(thread_wait2, NULL));
	CHECK_NOT_M1(close(fd));
	return EXIT_SUCCESS;
}
