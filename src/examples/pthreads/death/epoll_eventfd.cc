/*
 * This file is part of the demos-os-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-os-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-os-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-os-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <err_utils.h>

const int MAX_THREADS=5;
const int MAX_EVENTS=10;

typedef struct {
	pthread_t thread_id;
	int thread_num;
	int event_fd;
} thread_info_t;

// Worker thread function
void* worker_thread(void* arg) {
	thread_info_t* info = (thread_info_t*)arg;

	// Simulate some work with random duration
	int sleep_time = 1 + (rand() % 5);
	printf("Thread %d: Starting work (will run for %d seconds)\n", info->thread_num, sleep_time);

	sleep(sleep_time);
	printf("Thread %d: Work completed, signaling master\n", info->thread_num);
	// Signal completion by writing to eventfd
	uint64_t value = 1;
	CHECK_NOT_M1(write(info->event_fd, &value, sizeof(value)));
	return NULL;
}

int main() {
	thread_info_t threads[MAX_THREADS];
	struct epoll_event ev, events[MAX_EVENTS];
	int epoll_fd;
	int threads_alive = MAX_THREADS;

	// Seed random number generator
	srand(time(NULL));

	// Create epoll instance
	epoll_fd = CHECK_NOT_M1(epoll_create1(0));
	printf("Master: Creating %d worker threads\n", MAX_THREADS);

	// Create threads and their eventfds
	for(int i = 0; i < MAX_THREADS; i++) {
		threads[i].thread_num = i;
		// Create eventfd for this thread
		threads[i].event_fd = CHECK_NOT_M1(eventfd(0, EFD_CLOEXEC));
		// Add eventfd to epoll set
		ev.events = EPOLLIN;
		ev.data.ptr = &threads[i]; // Store thread info pointer
		CHECK_NOT_M1(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, threads[i].event_fd, &ev));
		// Create thread
		CHECK_ZERO_ERRNO(pthread_create(&threads[i].thread_id, NULL, worker_thread, &threads[i]));
	}
	printf("Master: All threads created, waiting for them to complete...\n\n");
	// Wait for threads to complete using epoll
	while (threads_alive > 0) {
		int nfds = CHECK_NOT_M1(epoll_wait(epoll_fd, events, MAX_EVENTS, -1));

		// Process completed threads
		for(int i = 0; i < nfds; i++) {
			thread_info_t* info = (thread_info_t*)events[i].data.ptr;
			if (events[i].events & EPOLLIN) {
				// Read from eventfd to clear the signal
				uint64_t value;
				CHECK_NOT_M1(read(info->event_fd, &value, sizeof(value)));
				printf("Master: Thread %d has completed (received signal)\n", info->thread_num);
				// Join the thread to clean up resources
				CHECK_ZERO_ERRNO(pthread_join(info->thread_id, NULL));
				// Remove from epoll set and close eventfd
				CHECK_NOT_ZERO(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, info->event_fd, NULL));
				CHECK_NOT_M1(close(info->event_fd));
				threads_alive--;
				printf("Master: %d threads still running\n", threads_alive);
			}
		}
	}
	printf("\nMaster: All threads have completed!\n");
	// Clean up
	close(epoll_fd);
	return EXIT_SUCCESS;
}
