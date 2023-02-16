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
#include <unistd.h>	// for fork(2)
#include <stdio.h>	// for fgets(3)
#include <sys/types.h>	// for waitid(2)
#include <sys/wait.h>	// for waitid(2)
#include <stdlib.h>	// for exit(3), atoi(3), EXIT_SUCCESS, EXIT_FAILURE
#include <string.h>	// for strsignal(3)
#include <trace_utils.h>// for TRACE()
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <multiproc_utils.h>	// for print_code(), print_status()

/*
 * This example explains how parents should wait for their children
 * and analyze the result of the childs death or the like.
 * The example uses the most complex interface waitid(2). You could,
 * ofcourse, alter it to use one of the simpler apis:
 * wait, waitpid, wait3, wait4...
 */

int main(int argc, char** argv, char** envp) {
	TRACE("this is the parent");
	pid_t child_pid=CHECK_NOT_M1(fork());
	if(child_pid==0) {
		bool selected=false;
		int selection;
		while(!selected) {
			TRACE("this is the child");
			TRACE("Please select what do you want to do:");
			TRACE("1) exit(EXIT_SUCCESS)");
			TRACE("2) exit(EXIT_FAILURE)");
			TRACE("3) do SIGSEGV");
			TRACE("- do 'kill -s SIGSTOP %d'", getpid());
			TRACE("- do 'kill -s SIGCONT %d'", getpid());
			char str[256];
			CHECK_CHARP(fgets(str, 256, stdin), str);
			selection=atoi(str);
			if ((selection>=1) && (selection<=3)) {
				selected=true;
			}
		}
		switch (selection) {
		case 1:
			exit(EXIT_SUCCESS);
			break;

		case 2:
			exit(EXIT_FAILURE);
			break;

		case 3:
			*((char *)0)=0;
			break;
		}
	} else {
		TRACE("this is the parent");
		bool over=false;
		while(!over) {
			TRACE("waiting for the child...");
			siginfo_t info;
			CHECK_NOT_M1(waitid(P_PID, child_pid, &info, WEXITED | WSTOPPED | WCONTINUED));
			print_code(info.si_code);
			print_status(info.si_status);
			if ((info.si_code==CLD_EXITED) || (info.si_code==CLD_KILLED)) {
				over=true;
			}
		}
	}
	return EXIT_SUCCESS;
}
