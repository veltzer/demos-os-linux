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
 * This is a classic example of fork/wait where the parent collects
 * the child return code in a signal handler which enables the
 * parent to continue doing calculations of some sort.
 * The problem with this approach is that we may lose SIGCHLD
 * signals since while we are in the signal handler signals may
 * accumulate and we are NOT guaranteed to get the correct number
 * of signals.
 */

#include <firstinclude.h>
#include <unistd.h>	// for fork(2)
#include <stdlib.h>	// for EXIT_SUCCCESS
#include <sys/types.h>	// for wait(3)
#include <sys/time.h>	// for wait(3)
#include <sys/resource.h>	// for wait(3)
#include <sys/wait.h>	// for wait(3)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ASSERT()
#include <multiproc_utils.h>	// for print_status()
#include <stdio.h>	// for printf(3)
#include <signal_utils.h>	// for signal_register_handler_sigaction()

static void handler(int sig, siginfo_t *si, void *context) {
	/*
	this is not good code (see above comment)

	int status;
	pid_t child_that_died=CHECK_NOT_M1(wait(&status));
	printf("child died with pid=%d\n", child_that_died);
	print_status(status);
	*/
	bool check_again=true;
	do {
		int status;
		// we don't check for errors since we may get ECHILD
		int ret=wait3(&status, WNOHANG, NULL);
		if(ret==-1) {
			if(errno==ECHILD) {
				check_again=false;
			} else {
				CHECK_NOT_M1(ret);
			}
		} else {
			// ret==0 means we have more live children, shouldnt
			// happen in this example
			CHECK_ASSERT(ret!=0);
			pid_t pid=ret;
			printf("child died with pid=%d\n", pid);
			print_status(status);
		}
	} while(check_again);
}

int main(int argc, char** argv, char** envp) {
	pid_t pid=CHECK_NOT_M1(fork());
	if(pid) {
		// parent
		signal_register_handler_sigaction(SIGCHLD, handler);
		// long calculation
		while(true) {
		}
		return EXIT_SUCCESS;
	} else {
		// child
		return 7;
	}
}
