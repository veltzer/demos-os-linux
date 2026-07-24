/*
 * This file is part of the demos-os-linux package.
 * Copyright (C) 2011-2026 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-os-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-os-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-os-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <unistd.h>	// for fork(2), pipe(2), close(2), read(2), write(2), sleep(3)
#include <stdio.h>	// for printf(3)
#include <sys/types.h>	// for waitid(2), kill(2)
#include <sys/wait.h>	// for waitid(2)
#include <signal.h>	// for kill(2)
#include <errno.h>	// for errno, ESRCH
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_ZERO(), CHECK_NOT_M1(), CHECK_1(), CHECK_ASSERT()
#include <trace_utils.h>// for TRACE()
#include <multiproc_utils.h>	// for my_system(), print_code(), print_status()

/*
 * This example extends the make_zombie example and shows that a zombie
 * only stays a zombie for as long as its parent is alive. Once the
 * parent dies the zombie is adopted by init(1) (or by the nearest
 * child reaper, see PR_SET_CHILD_SUBREAPER in prctl(2)) which
 * immediately waits for it and so clears it from the system.
 *
 * The cast:
 * - the observer (main process) which watches everything from the side.
 * - the parent, a child of the observer.
 * - the zombie, a child of the parent (a grandchild of the observer).
 *
 * The flow:
 * - the observer forks the parent and the parent forks the zombie.
 * - the zombie dies immediately. The parent, being a bad parent, never
 * waits for it, so it hangs around as a zombie.
 * - the observer shows the state of the zombie (Z).
 * - the observer tells the parent to die and reaps it.
 * - now init(1) adopts and clears the zombie: the observer shows that
 * the zombie is gone from the system altogether.
 */

int main() {
	// pipe for the parent to report the pid of the zombie to the observer
	int pipefd_report[2];
	CHECK_ZERO(pipe(pipefd_report));
	// pipe for the observer to tell the parent to die
	int pipefd_die[2];
	CHECK_ZERO(pipe(pipefd_die));

	pid_t parent_pid=CHECK_NOT_M1(fork());
	if(parent_pid==0) {
		// the parent
		CHECK_ZERO(close(pipefd_report[0]));
		CHECK_ZERO(close(pipefd_die[1]));
		pid_t zombie_pid=CHECK_NOT_M1(fork());
		if(zombie_pid==0) {
			// the zombie to be: die immediately
			return EXIT_SUCCESS;
		}
		// wait until the child is really dead but do NOT reap it
		// (WNOWAIT leaves the child in its zombie state)
		siginfo_t info;
		CHECK_NOT_M1(waitid(P_PID, zombie_pid, &info, WEXITED | WNOWAIT));
		// report the pid of the zombie to the observer
		CHECK_ASSERT(write(pipefd_report[1], &zombie_pid, sizeof(zombie_pid))==sizeof(zombie_pid));
		// wait for the observer to tell us to die...
		char c;
		CHECK_1(read(pipefd_die[0], &c, 1));
		// die without ever waiting for our child
		return EXIT_SUCCESS;
	} else {
		// the observer
		CHECK_ZERO(close(pipefd_report[1]));
		CHECK_ZERO(close(pipefd_die[0]));
		pid_t zombie_pid;
		CHECK_ASSERT(read(pipefd_report[0], &zombie_pid, sizeof(zombie_pid))==sizeof(zombie_pid));
		TRACE("the zombie is %d and its parent %d is still alive", zombie_pid, parent_pid);
		my_system("ps --no-headers -o pid,comm,state %d", zombie_pid);
		// tell the parent it is ok to die and collect it
		CHECK_1(write(pipefd_die[1], "d", 1));	// d is for die
		siginfo_t info;
		CHECK_NOT_M1(waitid(P_PID, parent_pid, &info, WEXITED));
		print_code(info.si_code);
		print_status(info.si_status);
		// give init(1) a moment to adopt and clear the zombie...
		CHECK_ZERO(sleep(1));
		// the zombie should be gone now: signaling it should fail with
		// ESRCH (no such process)
		TRACE("the parent is dead, checking on the zombie again...");
		CHECK_ASSERT(kill(zombie_pid, 0)==-1 && errno==ESRCH);
		TRACE("process %d is gone: init(1) adopted and cleared it", zombie_pid);
		return EXIT_SUCCESS;
	}
}
