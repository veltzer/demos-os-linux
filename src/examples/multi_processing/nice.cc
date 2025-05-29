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
#include <stdio.h>	// for printf(3)
#include <unistd.h>	// for nice(2), fork(2), pause(2)
#include <stdlib.h>	// for exit(3), EXIT_SUCCESS
#include <err_utils.h>	// CHECK_NOT_M1(), CHECK_ASSERT()

/*
 * This demo is to be used when demoing nice level.
 * Run a couple of these processes and look at the output.
 *
 * A few notes about this example:
 * - as you can see nice(2) is a "soft" priority. All of these processes are very
 * busy but still the ones with the bigger nice value manage to get a few
 * instructions executed at some point in time...
 * - from the command line you can start a process with a certain nice level
 * by using:
 * nice --adjustment=[nice level] [command with args]
 * or you can set a nice level of a process after it has started with the renice
 * command:
 *
 * You can see the nice level of your process via ps using the %n value:
 * ps -o %n,...
 *
 * You can see the nice level of a process via the /proc filesystem using
 * the following command:
 * cat /proc/[pid]/stat | cut -f 19 -d " "
 * (see man 5 proc for more details...)
 */

pid_t make_child(int nice_val) {
	pid_t child_pid=CHECK_NOT_M1(fork());
	if(child_pid==0) {
		// child process
		// set our own nice level to the required level...
		CHECK_NOT_M1(nice(nice_val));
		pid_t mypid=getpid();
		int i=0;
		while(true) {
			if (i % 10000000==0) {
				printf("pid %d, counter %d, nice %d\n", mypid, i, nice_val);
			}
			i++;
		}
		exit(EXIT_SUCCESS);
		return -1;
	} else {
		return child_pid;
	}
}

int main() {
	make_child(1);
	make_child(2);
	make_child(3);
	// a non busy wait busy wait loop...:)
	while(true) {
		int ret=pause();
		CHECK_ASSERT(ret==-1 && errno==EINTR);
	}
	return EXIT_SUCCESS;
}
