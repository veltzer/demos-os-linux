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
#include <unistd.h>	// for sleep(3)
#include <stdio.h>	// for printf(3)
#include <err_utils.h>	// for CHECK_ZERO_ERRNO(), CHECK_ZERO()
#include <pthread_utils.h>	// for gettid()
#include <signal_utils.h>	// for signal_register_handler_sigaction()

/*
 * This example explores the relations between threads and signal handling.
 * If we have more than one thread which thread will be used to handle a signal?
 *
 * Results:
 *
 * Notes:
 * - you can signal threads from the command line using kill(1). You just have
 * to know the thread id.
 * - If you register a signal handler from the main thread before the creation of
 * the child threads then they too will have the signal handler installed.
 * - You can also do it after the child threads are created.
 * - What if a signal is registered by the child? Same. All threads get the handler.
 *
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 *
 * TODO:
 * - write the results of this example in detail.
 */

// count the number of signals we get
static unsigned int counter=0;

static void handler(int sig, siginfo_t *si, void* unused __attribute__((unused))) {
	printf("sighandler: gettid() is %d\n", gettid());
	printf("sighandler: counter is %d\n", counter);
	printf("sighandler: got signal %s\n", strsignal(sig));
	printf("sighandler: si is %p\n", (void*)si);
	printf("sighandler: address is: 0x%lx\n", (long) si->si_addr);
	printf("sighandler: psiginfo follows...\n");
	psiginfo(si, "sighandler");
	counter++;
}

static void* worker(void* p) {
	int num=*(int *)p;
	printf("thread starting num=%d, gettid()=%d, pthread_self()=%lu\n", num, gettid(), pthread_self());
	if(num==0) {
		signal_register_handler_sigaction(SIGUSR1, handler, SA_SIGINFO);
	}
	// this is the right way to sleep even while accepting signals.
	// signal arrival will break the sleep and sleep will return number
	// of seconds remaining to sleep.
	int to_sleep=60;
	while((to_sleep=sleep(to_sleep))) {
		printf("wakeup because of signal\n");
	}
	printf("thread ending num=%d, gettid()=%d, pthread_self()=%lu\n", num, gettid(), pthread_self());
	return NULL;
}

int main() {
	const int num=2;
	pthread_t threads[num];
	int ids[num];
	void* rets[num];

	// signal_register_handler_sigaction(SIGUSR1,handler);
	printf("my pid is %d\n", getpid());
	printf("signal me with [kill -s SIGUSR1 %d]\n", getpid());
	printf("signal me with [kill -s SIGUSR2 %d]\n", getpid());

	printf("main starting\n");
	printf("main started creating threads\n");
	for(int i=0; i<num; i++) {
		ids[i]=i;
		CHECK_ZERO_ERRNO(pthread_create(threads + i, NULL, worker, ids + i));
	}
	printf("main ended creating threads\n");
	CHECK_ZERO(sleep(1));
	signal_register_handler_sigaction(SIGUSR2, handler, SA_SIGINFO);
	printf("main started joining threads\n");
	for(int i=0; i<num; i++) {
		CHECK_ZERO_ERRNO(pthread_join(threads[i], rets + i));
	}
	printf("main ended joining threads\n");
	printf("main ended\n");
	return EXIT_SUCCESS;
}
