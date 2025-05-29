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
#include <unistd.h>	// for fork(2), getpid(2), sleep(3), getppid(2)
#include <stdio.h>	// for fgets(3)
#include <sys/types.h>	// for waitid(2), getpid(2), getppid(2)
#include <sys/wait.h>	// for waitid(2)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <string.h>	// for strsignal(3)
#include <signal.h>	// for kill(2)
#include <trace_utils.h>// for TRACE()
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ZERO(), CHECK_ASSERT()

/*
 * This example demostrates what happens when a processes father dies...
 *
 * - Notice that the getppid() function returns different values because of the
 * adoption by the init(1) process.
 * - This example forks twice to show that there is no "grandparent adoption" and
 * that grandparents are not interested in their grandchildren.
 * - We need to wait a little to make sure that the parent dies - there is no synchroneous
 * way to get this info as far as I know...
 *
 * TODO:
 * - investigate how it is exactly that init does the things he does (is it the kernel as I
 * suspect that latches processes under the init process ?!? - if so, find the place
 * in the kernel that does this...).
 */

int main() {
	pid_t child_pid=CHECK_NOT_M1(fork());
	if (child_pid==0) {
		pid_t gchild_pid=CHECK_NOT_M1(fork());
		if (gchild_pid==0) {
			TRACE("this is the gchild, pid is %d", getpid());
			// lets show the parent pid...
			TRACE("my parent pid is %d", getppid());
			// now lets signal our parent that its ok to die...
			CHECK_NOT_M1(kill(getppid(), SIGUSR1));
			// lets wait a while to make sure the parent really dies..
			CHECK_ZERO(sleep(1));
			// now lets print our parent again...
			TRACE("my parent pid is %d", getppid());
			return 0;
		} else {
			TRACE("this is the parent, pid is %d", getpid());
			// lets wait for a signal that it's ok to die...
			int ret=pause();
			CHECK_ASSERT(ret==-1 && errno==EINTR);
			// lets die
			return 0;
		}
	} else {
		TRACE("this is the gparent, pid is %d", getpid());
		// lets wait for a signal that it's ok to die...
		int ret=pause();
		CHECK_ASSERT(ret==-1 && errno==EINTR);
		// lets die
		return 0;
	}
	return EXIT_SUCCESS;
}
