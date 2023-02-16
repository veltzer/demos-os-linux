/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for fprintf(3), stderr(object)
#include <signal.h>	// for psignal(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <unistd.h>	// for pause(2), getpid(2)
#include <sys/types.h>	// for getpid(2)
#include <string.h>	// for strsignal(3)
#include <err_utils.h>	// for CHECK_ASSERT(), CHECK_ZERO()
#include <signal_utils.h>	// for register_handler_signal()

/*
 * This example expands on the previous example by calling the signal handler
 * that was installed before the current one.
 * Note that the old handler for SIGUSR1 causes a segmentation fault.
 */

static unsigned int counter;
static sighandler_t old_handler;

static void handler(int sig) {
	counter++;
	psignal(sig, "handler");
	fprintf(stderr, "handler: start %d handler, sig is %d, name is %s\n", counter, sig, strsignal(sig));
	fprintf(stderr, "handler: calling old handler\n");
	old_handler(sig);
	fprintf(stderr, "handler: end\n");
}

int main(int argc, char** argv, char** envp) {
	// set up the signal handler (only need to do this once)
	old_handler=signal_register_handler_signal(SIGUSR1, handler);
	fprintf(stderr, "signal me with one of the following:\n");
	fprintf(stderr, "[kill -s SIGUSR1 %d]\n", getpid());
	// This is a non busy wait loop which only wakes up when there are signals
	while(true) {
		int ret=pause();
		CHECK_ASSERT(ret==-1 && errno==EINTR);
		fprintf(stderr, "pause(2) wakeup\n");
	}
	return EXIT_SUCCESS;
}
