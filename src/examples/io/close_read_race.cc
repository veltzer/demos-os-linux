/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <pthread.h>	// for pthread_t, pthread_create(3), pthread_join(3), pthread_self(3)
#include <unistd.h>	// for sleep(3), getpid(2)
#include <sys/types.h>	// for getpid(2)
#include <sched.h>	// for sched_getcpu(2)
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <err_utils.h>

/*
 * This is a standard pthread demo.
 * This shows the various attributes of each of the threads:
 * pthread_self, tid, tid cached, pid, core it is running
 * on.
 *
 * EXTRA_COMPILE_FLAGS=-g
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

int fd;

static void* closer(void*) {
	/* This is the thread that closes the file descritor */
	sleep(10);
	int ret=close(3);
	assert(ret!=-1);
	printf("\n\n\n\n\nclose was ok!\n\n\n\n");
	sleep(3600);
	return NULL;
}

static void* reader(void*) {
	char buf[10];
	while(1) {
		int count=CHECK_NOT_M1(read(3, buf, 10));
		CHECK_NOT_M1(write(1, "got data!\n", 10));
		CHECK_NOT_M1(write(1, buf, count));
	}
	return NULL;
}

int main() {
	const char* file="/dev/input/mouse5";
	fd=open(file, O_RDWR);
	pthread_t threads[2];
	pthread_create(threads, NULL, closer, NULL);
	pthread_create(threads+1, NULL, reader, NULL);
	pthread_join(*threads, NULL);
	pthread_join(threads[1], NULL);
	return EXIT_SUCCESS;
}
