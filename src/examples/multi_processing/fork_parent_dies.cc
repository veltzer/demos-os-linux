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
#include <unistd.h>	// for fork(2), getpid(2), getppid(2), sleep(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <stdio.h>	// for printf(3)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <sys/types.h>	// for getpid(2), getppid(2)

/*
 * This example shows what happens in fork(2) when the parent dies.
 * We make sure here that the parent dies before the child prints
 * all of it's output.
 * In original UNIX systems the parent of the child was supposed
 * to be changed to 1/init (adoption). In current Linux systems
 * the child will be adopted by systemd, upstart which may or
 * may not have pid=1 according to how those systems were designed.
 * On my Ubuntu system the parent is 'upstart --user' which is process
 * run to collect the dead per user on login.
 */

int main() {
	pid_t child_pid=CHECK_NOT_M1(fork());
	if(child_pid) {
		printf("%d: this is the parent\n", getpid());
		printf("%d: getpid()=%d\n", getpid(), getpid());
		printf("%d: getppid()=%d\n", getpid(), getppid());
		// let the parent die
	} else {
		printf("%d: this is the child\n", getpid());
		printf("%d: getpid()=%d\n", getpid(), getpid());
		printf("%d: getppid()=%d\n", getpid(), getppid());
		// wait for the parent to die
		sleep(5);
		printf("%d: this is the child\n", getpid());
		printf("%d: getpid()=%d\n", getpid(), getpid());
		printf("%d: getppid()=%d\n", getpid(), getppid());
	}
	return EXIT_SUCCESS;
}
