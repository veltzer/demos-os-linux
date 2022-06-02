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
#include <signal.h>	// for psignal(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <stdio.h>	// for fprintf(3), stderr(object)
#include <unistd.h>	// for pause(2), getpid(2), sleep(3)
#include <sys/types.h>	// for getpid(2)
#include <string.h>	// for strsignal(3)
#include <err_utils.h>	// for CHECK_ASSERT(), CHECK_ZERO()
#include <signal_utils.h>	// for register_handler_signal()

/*
 * This is an example of how to stop a thread/process which is doing 100%
 * CPU work
 */

static void handler(int sig) {
	fprintf(stderr, "Im inside the signal handler\n");
	exit(1);
}

int main(int argc, char** argv, char** envp) {
	// set up the signal handler (only need to do this once)
	signal_register_handler_signal(SIGUSR1, handler);
	fprintf(stderr, "signal me with one of the following:\n");
	fprintf(stderr, "[kill -s SIGUSR1 %d]\n", getpid());
	// lets not stop working...
	double sum=0;
	double n=1;
	while(true) {
		sum+=1.0f/(n*n);
		n+=1;
	}
	return EXIT_SUCCESS;
}
