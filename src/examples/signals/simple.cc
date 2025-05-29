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

/*
 * This is the most simple example of using signals on linux.
 * This shows that:
 * - signal handlers need to be installed just one time and will be trigger
 * every time the signal is delivered to the process.
 * - signals can wake you up when you are sleeping in the kernel and pull you
 * out back to user space to handle the signal in user space.
 */

#include <firstinclude.h>
#include <stdlib.h>	// for EXIT_SUCCESS
#include <stdio.h>	// for printf(3)
#include <signal.h>	// for signal(2)
#include <unistd.h>	// for sleep(3), getpid(2)
#include <err_utils.h>	// for CHECK_NOT_SIGT()

static void handler(int sig_num) {
	printf("Inside the signal handler %d in process %d\n", sig_num, getpid());
	printf("signal handler is done...\n");
}

int main() {
	CHECK_NOT_SIGT(signal(SIGUSR1, handler), SIG_ERR);
	printf("signal me with [kill -s SIGUSR1 %d]\n", getpid());
	printf("going to sleep...%d\n", getpid());
	sleep(3600);
	printf("waking up after the sleep...\n");
	return EXIT_SUCCESS;
}
