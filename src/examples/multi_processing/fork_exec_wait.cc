/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
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

/*
 * This is a classic example of the fork/exec/wait paradigm in Linux/Unix
 * multi processing.
 *
 * Notes:
 * - we use execl(3) because we have the full path of the executable and
 * do not want to waste time searching for it by using the PATH environment
 * variable.
 */

#include <firstinclude.h>
#include <unistd.h>	// for fork(2), execl(3)
#include <stdlib.h>	// for EXIT_SUCCCESS
#include <sys/types.h>	// for wait(2)
#include <sys/wait.h>	// for wait(2)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <multiproc_utils.h>	// for print_status()
#include <stdio.h>	// for printf(3)

int main(int argc, char** argv, char** envp) {
	pid_t pid=CHECK_NOT_M1(fork());
	if(pid) {
		// parent
		int status;
		pid_t child_that_died=CHECK_NOT_M1(wait(&status));
		printf("child died with pid=%d\n", child_that_died);
		print_status(status);
		return EXIT_SUCCESS;
	} else {
		// child
		CHECK_NOT_M1(execl("/bin/ls", "/bin/ls", "-l", NULL));
		return EXIT_SUCCESS;
	}
}
