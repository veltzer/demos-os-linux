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
#include <signal.h>	// for psignal(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <stdio.h>	// for fprintf(3), stderr(object)
#include <unistd.h>	// for getpid(2)
#include <sys/types.h>	// for getpid(2)
#include <signal_utils.h>	// for signal_register_handler_signal()

/*
 * This example shows that signal handling can break a process
 * from a busy wait loop, jump to the signal handling function
 * and then return to the busy wait code transparently.
 */

static void handler(int sig) {
	psignal(sig, "handler");
}

int main(int argc, char** argv, char** envp) {
	// set up the signal handler (only need to do this once)
	signal_register_handler_signal(SIGUSR1, handler);
	fprintf(stderr, "signal me with one of the following:\n");
	fprintf(stderr, "[kill -s SIGUSR1 %d]\n", getpid());
	// NOTE! This is a busy wait loop...
	while(true) {
	}
	return EXIT_SUCCESS;
}
