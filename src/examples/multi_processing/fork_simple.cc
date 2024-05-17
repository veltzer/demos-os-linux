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
#include <unistd.h>	// for fork(2), getpid(2), getppid(2), sleep(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <stdio.h>	// for printf(3)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <sys/types.h>	// for getpid(2), getppid(2)

/*
 * This example is the most simple type of fork.
 * the parent here doens't even wait to collect the return code of the child.
 * the sleep(3) is here to make sure that they stay alive long enough to print
 * the info correctly. Otherwise the parent may die before the child prints
 * in which case the child will get adopted.
 */

int main() {
	printf("%d: this is the parent\n", getpid());
	pid_t child_pid=CHECK_NOT_M1(fork());
	if(child_pid) {
		printf("%d: this is the parent\n", getpid());
		printf("%d: getpid()=%d\n", getpid(), getpid());
		printf("%d: getppid()=%d\n", getpid(), getppid());
		sleep(5);
	} else {
		printf("%d: this is the child\n", getpid());
		printf("%d: getpid()=%d\n", getpid(), getpid());
		printf("%d: getppid()=%d\n", getpid(), getppid());
		sleep(5);
	}
	return EXIT_SUCCESS;
}
