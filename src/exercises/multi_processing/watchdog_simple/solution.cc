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
#include <stdio.h>	// for printf(3), fprintf(3), stderr
#include <stdlib.h>	// for EXIT_SUCCESS, exit(3)
#include <unistd.h>	// for fork(2)
#include <sys/wait.h>
#include <errno.h>	// for errno
#include <string.h>	// for strerror(3)

int handle_error(int ret) {
	if(ret==-1) {
		int myerrno=errno;
		fprintf(stderr, "there was an error from a system call\n");
		fprintf(stderr, "the error was [%s]\n", strerror(myerrno));
		exit(-1);
		return -1;
	} else {
		return ret;
	}
}

int main() {
	printf("You will see this once...\n");
	printf("The parent pid is %d\n", getpid());
	bool need_to_run_child=true;
	while(need_to_run_child) {
		pid_t child_pid=handle_error(fork());
		if(child_pid) {
			printf("This is the parent\n");
			printf("The child pid is %d\n", child_pid);
			int status;
			handle_error(wait(&status));
			if(WIFEXITED(status)) {
				need_to_run_child=0;
			} else {
				fprintf(stderr, "Oops, the child died abnormaly and the signal with which it was killed was %d\n", WTERMSIG(status));
			}
		} else {
			printf("This is the child\n");
			sleep(10);
			// cppcheck-suppress nullPointer
			*(char*)0=0;
			return EXIT_SUCCESS;
		}
	}
	return EXIT_SUCCESS;
}
