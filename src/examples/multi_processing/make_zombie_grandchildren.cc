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
#include <unistd.h>	// for fork(2), pipe(2), close(2), read(2), write(2), sleep(3), getppid(2)
#include <stdio.h>	// for printf(3)
#include <sys/types.h>	// for waitid(2)
#include <sys/wait.h>	// for waitid(2)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_ZERO(), CHECK_NOT_M1(), CHECK_1()
#include <trace_utils.h>// for TRACE()
#include <multiproc_utils.h>	// for my_system(), print_code(), print_status()

/*
 * This example extends the make_zombie example and shows what happens
 * to the children of a zombie (the grandchildren of the main process
 * of this example).
 *
 * A zombie is dead: it will never wait for its own children. So the
 * kernel does not leave the children of a dying process in the hands
 * of the zombie it becomes: they are adopted by init(1) (or by the
 * nearest child reaper, see PR_SET_CHILD_SUBREAPER in prctl(2)) at
 * the moment their parent dies, even though the parent, as a zombie,
 * is still listed in the process table. When they die init(1) will
 * clear them, so no second generation of zombies accumulates behind
 * a zombie.
 *
 * The flow:
 * - the main process forks a child and the child forks a grandchild.
 * - the child dies. The main process does not wait for it (yet) so the
 * child becomes a zombie.
 * - the grandchild shows that its parent pid changed to init(1) even
 * though its original parent still exists in the system as a zombie.
 */

int main() {
	// pipe for the grandchild to tell the main process it is done
	int pipefd_done[2];
	CHECK_ZERO(pipe(pipefd_done));

	pid_t child_pid=CHECK_NOT_M1(fork());
	if(child_pid==0) {
		// the child
		CHECK_ZERO(close(pipefd_done[0]));
		pid_t gchild_pid=CHECK_NOT_M1(fork());
		if(gchild_pid==0) {
			// the grandchild
			TRACE("grandchild: my parent is %d", getppid());
			// let our parent die and become a zombie...
			CHECK_ZERO(sleep(1));
			// our parent is now a zombie and yet we were reparented
			// to init(1): a zombie does not keep its children
			TRACE("grandchild: my parent is now %d", getppid());
			// tell the main process we are done
			CHECK_1(write(pipefd_done[1], "d", 1));	// d is for done
			return EXIT_SUCCESS;
		}
		// the child dies immediately and becomes a zombie since the
		// main process will not reap it until the end of the example
		return EXIT_SUCCESS;
	} else {
		// the main process
		CHECK_ZERO(close(pipefd_done[1]));
		// wait for the grandchild to finish its show
		char c;
		CHECK_1(read(pipefd_done[0], &c, 1));
		// show that the child is STILL a zombie in the process table
		// while the grandchild already got adopted by init(1)
		my_system("ps --no-headers -o pid,comm,state %d", child_pid);
		// now reap the zombie
		siginfo_t info;
		CHECK_NOT_M1(waitid(P_PID, child_pid, &info, WEXITED));
		print_code(info.si_code);
		print_status(info.si_status);
		// we cannot wait for the grandchild - it is not our child, it
		// belongs to init(1) now which will clear it when it dies
		return EXIT_SUCCESS;
	}
}
