/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <unistd.h>	// for fork(2), getpid(2), getppid(2), sleep(3), setpgrp(2), getpgrp(2)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <stdio.h>	// for printf(3)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <sys/types.h>	// for getpid(2), getppid(2)
#include <linux/unistd.h> // for exit_group(2)

/*
 * This example investigates process groups.
 */

int main(int argc, char** argv, char** envp) {
	CHECK_NOT_M1(setpgrp());
	printf("%d: this is the parent\n", getpid());
	pid_t child_pid=CHECK_NOT_M1(fork());
	if(child_pid) {
		sleep(2);
		printf("%d: this is the parent\n", getpid());
		printf("%d: getpid()=%d\n", getpid(), getpid());
		printf("%d: getppid()=%d\n", getpid(), getppid());
		printf("%d: getpgrp()=%d\n", getpid(), getpgrp());
		printf("%d: the parent is dying...\n", getpid());
		CHECK_NOT_M1(syscall(__NR_exit_group, 0));
		// does not compile
		//exit_group(EXIT_SUCCESS);
	} else {
		for(int i=0;i<10;i++) {
			printf("%d: this is the child\n", getpid());
			printf("%d: getpid()=%d\n", getpid(), getpid());
			printf("%d: getppid()=%d\n", getpid(), getppid());
			printf("%d: getpgrp()=%d\n", getpid(), getpgrp());
			sleep(1);
		}
	}
	return EXIT_SUCCESS;
}
