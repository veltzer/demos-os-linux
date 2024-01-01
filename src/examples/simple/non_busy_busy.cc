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
#include <unistd.h>	// for pause(2)
#include <stdio.h>	// for printf(3)
#include <err_utils.h>	// for CHECK_ASSERT()
#include <sys/types.h>	// for getpid(2)
#include <unistd.h>	// for getpid(2)

/*
 * This example shows a 'non busy busy wait' loop.
 * The program wait's indefinately.
 * You could do the same with 'sleep(5)' inside the loop
 * but with the following disadvantages:
 * - you wake up every 5 seconds - for what?!? waste of time. On a mobile
 * phone this means you are draining the battery.
 * - by using pause(2) you wake up when interesting things happen (a signal
 * arrives). This is much better. And you don't wake up when there is nothing
 * interesting going on.
 *
 * NOTES:
 * - remember that with pause(2) you wake up in the main loop *after* the signal
 * has been handled.
 * - This example can be used for showing:
 * pmap `pidof [this executables name]
 * or:
 * cat /proc/`pidof [this executables name]`/maps
 * - if you unremark the array below you will see a much bigger
 * data segment for initialisation of {1} and a small data segment for initialisation of {0}.
 * This is because {0} blocks are lazily created at runtime.
 * You can see the size of the data segment by simply looking at the size of executable file.
 */

//int myarray[10000]={1};

int main(int argc, char** argv, char** envp) {
	printf("my pid is [%d]...\n", getpid());
	printf("signal me using [kill -s SIGUSR1 %d]...\n", getpid());
	while(true) {
		int ret=pause();
		CHECK_ASSERT(ret==-1 && errno==EINTR);
		printf("got some signal...\n");
	}
	return 0;
}
