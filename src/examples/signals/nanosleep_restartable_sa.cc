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

#include <firstinclude.h>
#include <stdio.h>	// for fprintf(3)
#include <unistd.h>	// for sleep(3)
#include <stdlib.h>	// for EXIT_SUCCESS, atoi(3)
#include <err_utils.h>	// for CHECK_ZERO()
#include <signal_utils.h>	// for signal_register_handler_sigaction()
#include <assert.h>	// for assert(3)
#include <errno.h>	// for errno, EINTR
#include <time.h>	// for time(2), localtime(3), struct timespec, nanosleep(2)

/*
 * This is an example of how to restart nanosleep(2) properly after a signal.
 *
 * Note: nanosleep(2) is NOT restartable via SA_RESTART - signals always interrupt
 * it (returning EINTR) even when the handler was registered with SA_RESTART.
 * The canonical way to "restart" nanosleep is to loop, passing the remaining
 * time back in, as demonstrated below.
 *
 * References:
 * - https://stackoverflow.com/questions/5198560/using-sleep-and-select-with-signals
 * - nanosleep(2) man page NOTES section
 */

static void handler(int signum, siginfo_t*, void*) {
	printf("got signal %d...\n", signum);
	// sleep(2);
}

static void print_current_time() {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	printf("now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

int main() {
	fprintf(stderr, "signal me with one of the following:\n");
	fprintf(stderr, "[kill -s SIGUSR1 %d]\n", getpid());
	signal_register_handler_sigaction(SIGUSR1, handler, SA_RESTART);
	struct timespec timeout;
	timeout.tv_sec = 30;
	timeout.tv_nsec = 0;
	struct timespec rem;
	print_current_time();
	while(nanosleep(&timeout, &rem) == -1 && errno == EINTR) {
		timeout = rem;
	}
	print_current_time();
	return EXIT_SUCCESS;
}
