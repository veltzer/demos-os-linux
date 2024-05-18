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
#include <signal.h>	// for siginterrupt(3)
#include <string.h>	// for strsignal(3)
#include <stdio.h>	// for fprintf(3)
#include <sys/types.h>	// for getpid(2)
#include <unistd.h>	// for getpid(2), read(2)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <trace_utils.h>// for TRACE()
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <signal_utils.h>	// for register_handler_signal()

/*
 * This demo demostrates how to cause a thread that is stuck in a long system call to
 * break out of it. The idea is to generate a signal and to define that signal as an
 * interrupt signal. This will not cause the system call to be restarted but rather
 * the system call (in this examples case read(2)) will return with an error(-1) and
 * the error code will be -EINTR. On receiving this the main thread will politely end
 * (throw an exception ?!?).
 */

static int counterUSR1=0;
static int counterUSR2=0;
static int flag=0;

static void SignalHandlerUSR1(int sig, siginfo_t *, void*) {
	counterUSR1++;
	fprintf(stderr, "handler [%s]: %d starting\n", strsignal(sig), counterUSR1);
}

static void SignalHandlerUSR2(int sig, siginfo_t *, void*) {
	counterUSR2++;
	fprintf(stderr, "handler [%s]: %d starting\n", strsignal(sig), counterUSR2);
	// reverse the flag
	if (flag==0) {
		flag=1;
	} else {
		flag=0;
	}
	fprintf(stderr, "handler [%s]: %d setting flag to %d\n", strsignal(sig), counterUSR2, flag);
	signal_register_handler_sigaction(SIGUSR1, SignalHandlerUSR1, SA_RESTART);
}

int main() {
	// set up the signal handler (only need to do this once)
	signal_register_handler_sigaction(SIGUSR1, SignalHandlerUSR1, 0);
	signal_register_handler_sigaction(SIGUSR2, SignalHandlerUSR2, 0);
	TRACE("set up the sig handler, lets start");
	TRACE("send signals to me using:");
	TRACE("kill -s SIGUSR1 %d", getpid());
	// This is a non busy wait loop which only wakes up when there
	// are signals
	while(true) {
		TRACE("before read");
		const int size=256;
		char buf[size];
		CHECK_NOT_M1(read(fileno(stdin), buf, size));
		TRACE("after read");
	}
	return EXIT_SUCCESS;
}
