/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <unistd.h>	// for fork(2), getpagesize(2), sleep(3)
#include <pthread.h>	// for pthread_mutexattr_init(3), pthread_mutexattr_destroy(3), pthread_mutex_init(3), pthread_mutex_lock(3), pthread_mutex_unlock(3), pthread_mutex_destroy(3), PTHREAD_PROCESS_SHARED, pthread_mutexattr_setrobust(3), PTHREAD_MUTEX_ROBUST
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_NOT_M1(), CHECK_NOT_VOIDP()
#include <sys/mman.h>	// for mmap(2), PROT_READ, PROT_WRITE, MAP_SHARED, MAP_ANONYMOUS, MAP_FAILED
#include <trace_utils.h>// for TRACE()
#include <sys/types.h>	// for wait(3)
#include <sys/time.h>	// for wait(3)
#include <sys/resource.h>	// for wait(3)
#include <sys/wait.h>	// for wait(3)
#include <multiproc_utils.h>	// for print_status()
#include <signal_utils.h>	// for signal_register_handler_sigaction()
#include <pthread_utils.h>	// for pthread_mutex_get_counter()

/*
 * This example explores the robustness of futexes.
 * The algorithm:
 * Create two processes with shared memory that hold a futex in shared memory.
 * One of the processes locks the lock and crashes.
 * The other tries to acquire the lock.
 *
 * Notes:
 * - if you do not set the mutex as robust (pthread_mutexattr_setrobust(3) below)
 * the the parent process will be stalled (the mutex will never be released).
 *
 * EXTRA_LINK_FLAGS=-lpthread
 */

static void handler(int sig, siginfo_t *si, void *context) {
	bool check_again=true;
	do {
		int status;
		// we don't check for errors since we may get ECHILD
		int ret=wait3(&status, WNOHANG, NULL);
		if(ret==-1) {
			if(errno==ECHILD) {
				check_again=false;
			} else {
				CHECK_NOT_M1(ret);
			}
		} else {
			// ret==0 means we have more live children, shouldnt
			// happen in this example
			CHECK_ASSERT(ret!=0);
			pid_t pid=ret;
			printf("child died with pid=%d\n", pid);
			print_status(status);
		}
	} while(check_again);
}

int main(int argc, char** argv, char** envp) {
	// setup shared memory where the futex will be
	void* shared=CHECK_NOT_VOIDP(mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0), MAP_FAILED);

	pthread_mutexattr_t attr;
	CHECK_ZERO_ERRNO(pthread_mutexattr_init(&attr));
	CHECK_ZERO_ERRNO(pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED));
	CHECK_ZERO_ERRNO(pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST));

	pthread_mutex_t* mylock=(pthread_mutex_t*)shared;
	// create the mutex with the SHARED attribute
	CHECK_ZERO_ERRNO(pthread_mutex_init(mylock, &attr));
	CHECK_ZERO_ERRNO(pthread_mutexattr_destroy(&attr));
	if(CHECK_NOT_M1(fork())) {
		TRACE("parent");
		signal_register_handler_sigaction(SIGCHLD, handler);
		sleep(1);
		TRACE("parent after sleep");
		// we do not error here automatically since we may get
		// an error code indicating that the last holder died
		// while holding the lock...
		int myerrno=pthread_mutex_lock(mylock);
		CHECK_ASSERT(myerrno==EOWNERDEAD);
		TRACE("parent confirmed that the previous owner of the lock died");
		// is the lock locked?
		CHECK_ASSERT(pthread_mutex_get_counter(mylock)==1);
		TRACE("parent confirmed that lock is now owned by the parent");
		TRACE("parent dies");
	} else {
		TRACE("child");
		CHECK_ZERO_ERRNO(pthread_mutex_lock(mylock));
		TRACE("child after lock");
		sleep(2);
		TRACE("child after sleep");
		*(volatile int*)0=0;
		TRACE("child after crash?!?");
		CHECK_ZERO_ERRNO(pthread_mutex_unlock(mylock));
		TRACE("child dies");
	}
	return EXIT_SUCCESS;
}
