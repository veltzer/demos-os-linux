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
#include <signal.h>	// for psignal(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <stdio.h>	// for fprintf(3), stderr(object)
#include <unistd.h>	// for pause(2), getpid(2), sleep(3)
#include <sys/types.h>	// for getpid(2)
#include <string.h>	// for strsignal(3)
#include <err_utils.h>	// for CHECK_ASSERT(), CHECK_ZERO()
#include <signal_utils.h>	// for register_handler_signal()

/*
 * This is a simple example which shows how to do signal handling with the
 * signal(2) syscall. Mind you that this is the old system call and there is a
 * better sigaction(2) syscall at your disposal.
 *
 * NOTES:
 * - the same signal handler can be used for handling more than one signal.
 * - if you send a signal when the signal handler is active then it is not
 * activated (meaning that the code that you write need not be re-entrant).
 * - the signal is remmember though and will be activated the first thing after
 * the signal handler is over.
 * - the system does not remmember more than 1 signal. It actually has a bit
 * mask of waiting signals so each waiting signal is either on or off.
 * - the pause(2) call can be used to block until a signal arrives an provide
 * you with a non-busy wait loop on signals.
 * - if you get a signal while inside a signal handler, then the main thread
 * (the one waiting on the pause(2) syscall) will not wake up until you deal
 * with the second one.
 * - if you use sigqueue to signal the process and use a real time signal
 * (SIGRTMIN - SIGRTMAX) and register on them then you will get full guarantee
 * of delivery (unless you congest the os rt signal queue that is - see ulimit
 * -r for details).
 */

static unsigned int counter=0;

static void handler(int sig) {
	counter++;
	psignal(sig, "handler");
	fprintf(stderr, "handler: start %d handler, sig is %d, name is %s\n", counter, sig, strsignal(sig));
	fprintf(stderr, "handler: sleeping for 10 seconds...\n");
	// CHECK_ZERO(sleep(10));
	fprintf(stderr, "handler: end\n");
}

int main() {
	// set up the signal handler (only need to do this once)
	signal_register_handler_signal(SIGUSR1, handler);
	signal_register_handler_signal(SIGUSR2, handler);
	signal_register_handler_signal(SIGRTMIN, handler);
	fprintf(stderr, "signal me with one of the following:\n");
	fprintf(stderr, "[kill -s SIGUSR1 %d]\n", getpid());
	fprintf(stderr, "[kill -s SIGUSR2 %d]\n", getpid());
	fprintf(stderr, "[kill -s SIGRTMIN %d]\n", getpid());
	// This is a non busy wait loop which only wakes up when there are signals
	while(true) {
		int ret=pause();
		CHECK_ASSERT(ret==-1 && errno==EINTR);
		fprintf(stderr, "pause(2) wakeup\n");
	}
	return EXIT_SUCCESS;
}
