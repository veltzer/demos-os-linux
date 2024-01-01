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
#include <stdio.h>	// for fprintf(3), fflush(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <unistd.h>	// for sleep(3), close(2)
#include <pthread.h>	// for pthread_create(3), pthread_join(3)
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <sys/ioctl.h>	// for ioctl(2)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ZERO_ERRNO()
#include <multiproc_utils.h>	// for my_system()
#include "shared.h"	// for ioctl numbers

/*
 * This application has at least two threads:
 *	- one running like crazy doing ioctls (and stopping when there are too many errors).
 *	- one asking to close the driver (which halts).
 *	- main which joins these threads (waits for them to die...)
 *
 * What do we learn from this?
 *	- closing a file descriptor only schedules release to be called in the kernel.
 *	- this release will be called once any ioctls on the current file descriptor end.
 *	- any new operations on this file descriptor are not allowed (bad file descriptor).
 *	- makes it easier to program in the kernel.
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

// file descriptor
int fd;

void *function_empty(void *p) {
	bool over=false;
	int counter=0;
	int errors=0;
	while(!over) {
		counter++;
		bool err;
		// ioctl to do nothing...
		int res=ioctl(fd, IOCTL_RACE_EMPTY, NULL);
		if (res==-1) {
			err=true;
			errors++;
		} else {
			err=false;
		}
		if (counter % 10000==0) {
			char c;
			if (err) {
				c='E';
			} else {
				c='.';
			}
			fprintf(stdout, "%c", c);
			fflush(stdout);
		}
		if(errors==100000) {
			over=true;
			fprintf(stdout, "X");
			fflush(stdout);
		}
	}
	return NULL;
}

void *function_short(void *p) {
	fprintf(stdout, "ss");
	fflush(stdout);
	CHECK_NOT_M1(ioctl(fd, IOCTL_RACE_SLEEP_SHORT, NULL));
	fprintf(stdout, "fs");
	fflush(stdout);
	return NULL;
}

void *function_long(void *p) {
	fprintf(stdout, "sl");
	fflush(stdout);
	CHECK_NOT_M1(ioctl(fd, IOCTL_RACE_SLEEP_LONG, NULL));
	fprintf(stdout, "fl");
	fflush(stdout);
	return NULL;
}

void *function_close(void *p) {
	CHECK_ZERO(sleep(2));
	fprintf(stdout, "c");
	fflush(stdout);
	CHECK_NOT_M1(close(fd));
	fprintf(stdout, "C");
	fflush(stdout);
	return NULL;
}

int main(int argc, char** argv, char** envp) {
	const char *filename="/dev/mod_ioctl_close_race";
	printf("Inserting the driver...\n");
	my_system("sudo rmmod mod_ioctl_close_race");
	my_system("sudo insmod ./mod_ioctl_close_race.ko");
	my_system("sudo chmod 666 %s", filename);

	fd=CHECK_NOT_M1(open(filename, O_RDWR));

	pthread_t thread_empty, thread_short, thread_long, thread_close;
	CHECK_ZERO_ERRNO(pthread_create(&thread_empty, NULL, function_empty, NULL));
	CHECK_ZERO_ERRNO(pthread_create(&thread_short, NULL, function_short, NULL));
	CHECK_ZERO_ERRNO(pthread_create(&thread_long, NULL, function_long, NULL));
	CHECK_ZERO_ERRNO(pthread_create(&thread_close, NULL, function_close, NULL));
	CHECK_ZERO_ERRNO(pthread_join(thread_empty, NULL));
	CHECK_ZERO_ERRNO(pthread_join(thread_short, NULL));
	CHECK_ZERO_ERRNO(pthread_join(thread_long, NULL));
	CHECK_ZERO_ERRNO(pthread_join(thread_close, NULL));
	fprintf(stdout, "\nALL DONE\n");
	return EXIT_SUCCESS;
}
