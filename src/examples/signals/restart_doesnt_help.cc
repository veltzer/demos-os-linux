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
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <assert.h>

/*
 * This is an example of syscall restarting after signal handling
 * This shows that SA_RESTART doesn't help every system call.
 */

static void handler(int sig_num, siginfo_t *, void*) {
	printf("Inside the signal handler %d in process %d\n", sig_num, getpid());
	printf("signal handler is done...\n");
}

int main() {
	// signal registration
	struct sigaction sa;
	sa.sa_flags=SA_RESTART;
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction=handler;
	struct sigaction old;
	int ret=sigaction(SIGUSR1, &sa, &old);
	assert(ret!=-1);

	// main code
	printf("going to sleep...%d\n", getpid());
	sleep(3600);
	printf("waking up after the sleep...\n");
}
