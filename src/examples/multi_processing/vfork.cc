/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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

#include <firstinclude.h>
#include <unistd.h>	// for fork(2), _exit(2)
#include <stdio.h>	// for fgets(3)
#include <sys/types.h>	// for waitid(2)
#include <sys/wait.h>	// for waitid(2)
#include <stdlib.h>	// for exit(3), atoi(3), EXIT_SUCCESS, EXIT_FAILURE
#include <string.h>	// for strsignal(3)
#include <sys/types.h>	// for vfork(2)
#include <unistd.h>	// for vfork(2)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ZERO()
#include <multiproc_utils.h>	// for print_code(), print_status()

/*
 * An example of using vfork(2)
 *
 * Notes:
 * - if you let the child return from main it will cause a segfault.
 * - this is ok because the documentation of vfork states explicitly that you
 * should not return from the function.
 * - tracing seems to work fine and sleep(3) also.
 * - writing to the parent data does not seem to be harmful. The parent sees these
 * writes as the pages are shared.
 * - result: the return from the function in which vfork was called is the one
 * causing the segfault.
 * - the segfault seems to be in the parent not in the child. Need to check this
 * for sure.
 * - the results of using vfork incorrectly (not for exec) are indeed undefined as the manual page says.
 */
int global_data=42;

int main(int argc, char** argv, char** envp) {
	TRACE("this is the parent. global_data is %d", global_data);
	pid_t child_pid=CHECK_NOT_M1(vfork());
	if (child_pid==0) {
		TRACE("this is the child");
		global_data++;
		// while(true) {
		// }
		//CHECK_ZERO(sleep(10));
		TRACE("child is dying...");
		_exit(0);
	} else {
		TRACE("this is the parent. global_data is %d", global_data);
		siginfo_t info;
		CHECK_NOT_M1(waitid(P_PID, child_pid, &info, WEXITED));
		print_code(info.si_code);
		print_status(info.si_status);
		TRACE("parent is dying...");
	}
	return EXIT_SUCCESS;
}
