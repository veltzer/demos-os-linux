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
#include <unistd.h>	// for fork(2), pause(2)
#include <stdio.h>	// for printf(3)
#include <sys/types.h>	// for wait(2), kill(2)
#include <sys/wait.h>	// for wait(2)
#include <signal.h>	// for SIGCHLD, SA_NOCLDWAIT, kill(2)
#include <string.h>	// for strsignal(3)
#include <errno.h>	// for errno, ECHILD, ESRCH
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ASSERT()
#include <trace_utils.h>// for TRACE()
#include <signal_utils.h>	// for signal_register_handler_sigaction()

/*
 * This example shows how to use the SA_NOCLDWAIT flag when setting up
 * a signal handler for SIGCHLD in order to avoid zombies.
 *
 * When SIGCHLD is handled with SA_NOCLDWAIT the kernel does not turn
 * dead children into zombies at all: they are cleared from the system
 * immediately when they die and their exit status is discarded. This
 * is for parents which are not interested in the exit status of their
 * children and do not want to bother calling one of the wait(2) family
 * of functions just to avoid leaving zombies behind.
 *
 * Notes:
 * - the handler itself is still called, we just cannot wait from it:
 * wait(2) returns ECHILD since there is no zombie to collect.
 * - if you do not even want the notification you can set the SIGCHLD
 * disposition to SIG_IGN together with SA_NOCLDWAIT (or on Linux plain
 * SIG_IGN for SIGCHLD behaves like that too, see sigaction(2)).
 */

static void handler(int sig, siginfo_t *si, void* unused __attribute__((unused))) {
	printf("sighandler: got signal %s\n", strsignal(sig));
	printf("sighandler: child that died is %d\n", si->si_pid);
}

int main() {
	// register a SIGCHLD handler with SA_NOCLDWAIT so that our dead
	// children never become zombies (SA_SIGINFO is for getting the pid
	// of the dead child in the siginfo_t argument of the handler)
	signal_register_handler_sigaction(SIGCHLD, handler, SA_NOCLDWAIT | SA_SIGINFO);
	pid_t child_pid=CHECK_NOT_M1(fork());
	if(child_pid==0) {
		// the child dies immediately
		return EXIT_SUCCESS;
	} else {
		// the parent
		// wait for the SIGCHLD to arrive
		int ret=pause();
		CHECK_ASSERT(ret==-1 && errno==EINTR);
		// the child is completely gone - it did not become a zombie:
		// signaling it fails with ESRCH (no such process)
		CHECK_ASSERT(kill(child_pid, 0)==-1 && errno==ESRCH);
		TRACE("process %d is gone, no zombie was created", child_pid);
		// and there is no status to collect either: wait(2) says we
		// have no children to wait for
		int status;
		CHECK_ASSERT(wait(&status)==-1 && errno==ECHILD);
		TRACE("wait(2) returned ECHILD, nothing to collect");
		return EXIT_SUCCESS;
	}
}
