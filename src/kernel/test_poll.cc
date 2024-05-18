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
#include <stdio.h>	// for fprintf(3), fflush(3), fgets(3), stdout, snprintf(3)
#include <stdlib.h>	// for atoi(3), EXIT_SUCCESS
#include <unistd.h>	// for close(2)
#include <pthread.h>	// for pthread_create(3), pthread_join(3)
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <sys/ioctl.h>	// for ioctl(2)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ZERO_ERRNO(), CHECK_NOT_NULL()
#include <multiproc_utils.h>	// for my_system()
#include <trace_utils.h>// for INFO()
#include <epoll_utils.h>// for print_events()
#include <sys/epoll.h>	// for epoll_create(2), epoll_ctl(2), epoll_wait(2)
#include "shared.h"	// for ioctl numbers

/*
 * This is a test program for the poll driver.
 * It has two threads:
 * - first thread goes to sleep on a number of file descriptor it got from the driver.
 * - second thread gets fd numbers from the user and wakes up that specific fd.
 *
 * TODO:
 * - the ending is kind of botched. We need to end the program by signalling on an eventfd
 *	and not on one of the sleeping fds...I was lazy...
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

/*
 * data for the threads
 */
const int NUM_OF_SLEEPERS=10;
typedef struct _thread_data{
	int fd[NUM_OF_SLEEPERS];
	bool over;
} thread_data;

// do select in a loop until the program is over
static void *function_wait(void *p) {
	INFO("start");
	thread_data* pd=(thread_data*)p;

	const unsigned int max_events=10;
	// any value above 0 will do for epoll_create(2)
	int epollfd=CHECK_NOT_M1(epoll_create(1));
	for(int i=0; i<NUM_OF_SLEEPERS; i++) {
		int curr_fd=pd->fd[i];
		struct epoll_event my_ev;
		// my_ev.events=EPOLLIN | EPOLLET;
		my_ev.events=EPOLLIN;
		my_ev.data.fd=curr_fd;
		CHECK_NOT_M1(epoll_ctl(epollfd, EPOLL_CTL_ADD, curr_fd, &my_ev));
	}
	while(!pd->over) {
		struct epoll_event events[max_events];
		int nfds=CHECK_NOT_M1(epoll_wait(epollfd, events, max_events, -1));
		printf("woke up with %d events...\n", nfds);
		for(int n=0; n<nfds; n++) {
			int curr_fd=events[n].data.fd;
			uint32_t curr_mask=events[n].events;
			for(int i=0; i<NUM_OF_SLEEPERS; i++) {
				if (curr_fd==pd->fd[i]) {
					printf("got activity on fd number %d (real fd=%d)\n", i, curr_fd);
					const unsigned int bufsize=1024;
					char printbuff[bufsize];
					print_events(printbuff, bufsize, curr_mask);
					printf("got events %s\n", printbuff);
				}
			}
		}
	}
	INFO("end");
	return NULL;
}

// read a number from the user and wake up that file descriptor
static void *function_wakeup(void *p) {
	INFO("start\n");
	thread_data* pd=(thread_data*)p;
	while(!pd->over) {
		printf("1) wake up fd\n");
		printf("2) reset fd\n");
		printf("3) exit\n");
		printf("selection --> ");
		fflush(stdout);
		char str[256];
		CHECK_NOT_NULL(fgets(str, 256, stdin));
		int selection=atoi(str);
		if (selection<1 || selection>3) {
			printf("bad number %d...\n", selection);
			continue;
		}
		if (selection==1 || selection==2) {
			printf("give me a number of a fd. [0-%d): ", NUM_OF_SLEEPERS);
			fflush(stdout);
			char buf[256];
			CHECK_NOT_NULL(fgets(buf, 256, stdin));
			int fdnum=atoi(buf);
			if (fdnum<0 || fdnum>=NUM_OF_SLEEPERS) {
				printf("bad number %d...\n", fdnum);
				continue;
			}
			int fd=pd->fd[fdnum];
			if (selection==1) {
				printf("waking up fd number %d (real fd=%d)...\n", fdnum, fd);
				CHECK_NOT_M1(ioctl(fd, IOCTL_EPOLL_WAKE, NULL));
			}
			if (selection==2) {
				printf("resetting fd number %d (real fd=%d)...\n", fdnum, fd);
				CHECK_NOT_M1(ioctl(fd, IOCTL_EPOLL_RESET, NULL));
			}
		}
		if (selection==3) {
			printf("stopping...\n");
			pd->over=true;
			// wake up the other thread (UGLY!!!)
			CHECK_NOT_M1(ioctl(pd->fd[0], IOCTL_EPOLL_WAKE, NULL));
			continue;
		}
	}
	INFO("end\n");
	return NULL;
}

int main() {
	const char* basename="poll";
	char filename[256];
	snprintf(filename, 256, "/dev/mod_%s", basename);
	/*
	 * const char* ko_filename="src/kernel/mod_poll.ko";
	 * printf("Inserting the driver...\n");
	 * //my_system("sudo /sbin/rmmod mod_%s", basename);
	 * my_system("sudo /sbin/insmod %s", ko_filename);
	 * my_system("sudo chmod 666 %s", filename);
	 */

	thread_data td;
	td.over=false;
	for(unsigned int i=0; i<NUM_OF_SLEEPERS; i++) {
		td.fd[i]=CHECK_NOT_M1(open(filename, O_RDWR));
	}
	pthread_t thread_wait, thread_wakeup;
	CHECK_ZERO_ERRNO(pthread_create(&thread_wait, NULL, function_wait, &td));
	CHECK_ZERO_ERRNO(pthread_create(&thread_wakeup, NULL, function_wakeup, &td));
	CHECK_ZERO_ERRNO(pthread_join(thread_wait, NULL));
	CHECK_ZERO_ERRNO(pthread_join(thread_wakeup, NULL));
	// close all file descriptors
	for(unsigned int i=0; i<NUM_OF_SLEEPERS; i++) {
		CHECK_NOT_M1(close(td.fd[i]));
	}
	fprintf(stdout, "\nALL DONE\n");
	return EXIT_SUCCESS;
}
