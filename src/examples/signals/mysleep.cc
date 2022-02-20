/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <signal.h>	// for SIGALRM
#include <stdio.h>	// for printf(3), fprintf(3)
#include <unistd.h>	// for alarm(2), unistd(2)
#include <stdlib.h>	// for EXIT_SUCCESS, atoi(3)
#include <err_utils.h>	// for CHECK_ZERO()
#include <signal_utils.h>	// for register_handler_sigaction()

/*
 * This is an example of how to implement the sleep function using
 * alarm(2) and pause(2).
 * The real sleep(3) function is indeed implemented in a similar (albeit
 * more professional) way.
 * The sleep(3) function is considered a bad function in several aspects:
 * - first it's resolution is in seconds which is really not adequate for many
 * of today's uses.
 * - second it's use of signals may interfere with other stuff running on the
 * system.
 * In case of need use nanosleep(2) instead.
 *
 * TODO:
 * - only wait for SIGALRM in the mysleep() function.
 */

static void handler(int sig, siginfo_t *si, void *unused) {
	// do nothing?!?
}

void mysleep(int seconds) {
	CHECK_ZERO(alarm(seconds));
	int ret=pause();
	CHECK_ASSERT(ret==-1 && errno==EINTR);
}

int main(int argc, char** argv, char** envp) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [seconds]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	// parameters
	int seconds=atoi(argv[1]);
	// register the signal handler for SIGALRM
	// we have to do this since the default handler for SIGALRM
	// will terminate the running program
	signal_register_handler_sigaction(SIGALRM, handler, 0);
	// demonstrate the use of our sleep function
	while(true) {
		printf("before sleep\n");
		mysleep(seconds);
		printf("woke up\n");
	}
	return EXIT_SUCCESS;
}
