/*
 * This file is part of the demos-os-linux package.
 * Copyright (C) 2011-2026 Mark Veltzer <mark.veltzer@gmail.com>
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

/*
 * monitor_via_epoll_pidfd.c
 *
 * Demonstrates how to use pidfd_open() and epoll() in the *main thread*
 * to monitor for a child thread's termination.
 *
 * This example shows:
 * 1. The main thread, which also acts as the "monitor".
 * 2. A "target" thread that runs for a few seconds and exits.
 *
 * The main thread:
 * a. Creates the target thread.
 * b. Gets the target's kernel-level Thread ID (TID).
 * c. Creates a pidfd (process file descriptor) for that TID.
 * d. Adds the pidfd to an epoll instance.
 * e. Waits on epoll_wait() for the thread to exit.
 * f. Joins the thread to clean up resources.
 *
 * NOTE: This requires _GNU_SOURCE to be defined for syscall wrappers
 * and modern Linux-specific features.
 *
 * How to compile:
 * gcc -o pidfd_epoll_monitor pidfd_epoll_monitor.c -pthread
 *
 * How to run:
 * ./pidfd_epoll_monitor
 */

#include <firstinclude.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/pidfd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <err_utils.h>

pid_t gettid(void) {
	return (pid_t)syscall(SYS_gettid);
}

int pidfd_open(pid_t pid, unsigned int flags) __THROW {
	return syscall(SYS_pidfd_open, pid, flags);
}

// Data structure to share the target's TID with the monitor
typedef struct {
	pid_t target_tid;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int tid_ready;
} shared_data_t;

/**
 * @brief The "target" thread function (the one to be monitored).
 *
 * This thread gets its own TID, shares it with the monitor thread,
 * sleeps for 5 seconds, and then exits.
 */
void* target_thread_func(void *arg) {
	shared_data_t *data = (shared_data_t *)arg;
	pid_t my_tid = gettid();
	printf("Target thread (TID: %d): Started.\n", my_tid);
	CHECK_ZERO_ERRNO(pthread_mutex_lock(&data->mutex));
	data->target_tid = my_tid;
	data->tid_ready = 1;
	CHECK_ZERO_ERRNO(pthread_cond_signal(&data->cond));
	CHECK_ZERO_ERRNO(pthread_mutex_unlock(&data->mutex));
	printf("Target thread (TID: %d): Working for 5 seconds...\n", my_tid);
	sleep(5);
	printf("Target thread (TID: %d): Exiting now.\n", my_tid);
	return NULL;
}

/**
 * @brief Main function
 * Initializes shared data, creates the target thread,
 * and monitors it using pidfd and epoll.
 */
int main() {
	printf("Main (PID: %d, TID: %d): Starting program.\n", getpid(), gettid());
	shared_data_t data;
	CHECK_ZERO_ERRNO(pthread_mutex_init(&data.mutex, NULL));
	CHECK_ZERO_ERRNO(pthread_cond_init(&data.cond, NULL));
	data.tid_ready = 0;
	data.target_tid = 0;
	printf("Main: Creating target thread...\n");
	pthread_t tid_target;
	CHECK_ZERO_ERRNO(pthread_create(&tid_target, NULL, target_thread_func, &data));
	printf("Main: Waiting for target's TID...\n");
	CHECK_ZERO_ERRNO(pthread_mutex_lock(&data.mutex));
	while (data.tid_ready == 0) {
		CHECK_ZERO_ERRNO(pthread_cond_wait(&data.cond, &data.mutex));
	}
	pid_t target_tid = data.target_tid;
	CHECK_ZERO_ERRNO(pthread_mutex_unlock(&data.mutex));
	printf("Main: Got target TID: %d. Setting up monitoring...\n", target_tid);
	int pid_fd = CHECK_NOT_M1(pidfd_open(target_tid, 0));
	printf("Main: pidfd_open() successful (fd=%d).\n", pid_fd);
	int epoll_fd = CHECK_NOT_M1(epoll_create1(0));
	printf("Main: epoll_create1() successful (fd=%d).\n", epoll_fd);
	struct epoll_event event;
	event.events = EPOLLIN; // Thread/process exit is signaled via EPOLLIN
	event.data.fd = pid_fd;
	CHECK_NOT_M1(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pid_fd, &event));
	printf("Main: Added pidfd to epoll. Waiting for target to exit...\n");
	struct epoll_event events[1];
	int n=CHECK_NOT_M1(epoll_wait(epoll_fd, events, 1, -1));
	if (n > 0 && events[0].data.fd == pid_fd) {
		printf("Main: epoll_wait() returned. Detected exit of target thread %d.\n", target_tid);
	} else {
		printf("Main: epoll_wait() returned unexpected event.\n");
	}
	CHECK_NOT_M1(close(pid_fd));
	CHECK_NOT_M1(close(epoll_fd));
	printf("Main: Monitor fds closed.\n");
	printf("Main: Joining target thread...\n");
	CHECK_ZERO_ERRNO(pthread_join(tid_target, NULL));
	printf("Main: Target thread joined.\n");
	CHECK_ZERO_ERRNO(pthread_mutex_destroy(&data.mutex));
	CHECK_ZERO_ERRNO(pthread_cond_destroy(&data.cond));
	printf("Main: Program finished.\n");
	return EXIT_SUCCESS;
}
