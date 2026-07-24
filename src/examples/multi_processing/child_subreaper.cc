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
#include <unistd.h>	// for fork(2), getpid(2), getppid(2), sleep(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <sys/types.h>	// for wait(2)
#include <sys/wait.h>	// for wait(2)
#include <sys/prctl.h>	// for prctl(2)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ZERO()
#include <trace_utils.h>// for TRACE()
#include <multiproc_utils.h>	// for print_status()

/*
 * This example shows how to use prctl(2) with PR_SET_CHILD_SUBREAPER
 * to become a "child reaper" for all orphaned descendant processes.
 *
 * Normally when a process dies its children are adopted by init(1).
 * But if some ancestor of the dying process has marked itself as a
 * child reaper using PR_SET_CHILD_SUBREAPER then the orphans are
 * adopted by that ancestor instead. This is how service managers
 * like systemd(1) keep track of services that double fork.
 *
 * The flow of this example:
 * - the main process marks itself as a child reaper and forks a child.
 * - the child forks a grandchild and dies immediately.
 * - the grandchild is now an orphan. Without PR_SET_CHILD_SUBREAPER it
 * would have been adopted by init(1) and its parent pid would become 1.
 * Instead it is adopted by the main process.
 * - the main process, which only forked ONE process, gets to wait(2)
 * for TWO processes: its own child and the adopted grandchild.
 */

int main() {
	TRACE("reaper: my pid is %d", getpid());
	// mark ourselves as a child reaper for all our orphaned descendants
	CHECK_NOT_M1(prctl(PR_SET_CHILD_SUBREAPER, 1));
	pid_t child_pid=CHECK_NOT_M1(fork());
	if(child_pid==0) {
		// the child
		TRACE("child: my pid is %d", getpid());
		pid_t gchild_pid=CHECK_NOT_M1(fork());
		if(gchild_pid==0) {
			// the grandchild
			TRACE("grandchild: my parent is %d", getppid());
			// let our parent (the child) die...
			CHECK_ZERO(sleep(1));
			// now we are adopted by the reaper and NOT by init(1)...
			TRACE("grandchild: my parent is now %d", getppid());
			return EXIT_SUCCESS;
		} else {
			// the child dies immediately, orphaning the grandchild
			TRACE("child: dying and orphaning the grandchild");
			return EXIT_SUCCESS;
		}
	} else {
		// the reaper: we forked one process but we will reap two...
		for(int i=0; i<2; i++) {
			int status;
			pid_t pid_that_died=CHECK_NOT_M1(wait(&status));
			if(pid_that_died==child_pid) {
				TRACE("reaper: reaped my own child %d", pid_that_died);
			} else {
				TRACE("reaper: reaped the adopted grandchild %d", pid_that_died);
			}
			print_status(status);
		}
		return EXIT_SUCCESS;
	}
}
