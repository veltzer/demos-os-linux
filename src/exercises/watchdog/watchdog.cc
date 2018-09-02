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

/*
 * This is a solution to the watchdog exercise
 *
 * This is a simple solution. In this one the parent
 * just blocks and wait for the child to die using wait(2).
 */

#include <firstinclude.h>
#include <unistd.h>	// for fork(2), execv(2)
#include <stdlib.h>	// for EXIT_SUCCCESS
#include <sys/types.h>	// for wait(2), WIFSIGNALED(3)
#include <sys/wait.h>	// for wait(2), WIFSIGNALED(3)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <trace_utils.h>// for TRACE()
#include <multiproc_utils.h>	// for print_status()

const char* process_to_exec="src/exercises/watchdog/process_to_monitor.elf";
const char* const args[]={
	process_to_exec
};

static void fork_a_child() {
	TRACE("forking a child");
	pid_t pid=CHECK_NOT_M1(fork());
	if(pid) {
		// parent
		return;
	}
	// child
	CHECK_NOT_M1(execv(process_to_exec, (char* const*)args));
}

int main(int argc, char** argv, char** envp) {
	fork_a_child();
	TRACE("parent starts monitoring");
	while(true) {
		int status;
		pid_t child_that_died=CHECK_NOT_M1(wait(&status));
		TRACE("child died with pid=%d", child_that_died);
		print_status(status);
		if(WIFSIGNALED(status)) {
			fork_a_child();
		}
	}
	return EXIT_SUCCESS;
}
