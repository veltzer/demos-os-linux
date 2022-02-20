/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <unistd.h>	// for fork(2), pause(2)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <string.h>	// for strsignal(3)
#include <trace_utils.h>// for TRACE()
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ZERO(), CHECK_ASSERT()
#include <sys/prctl.h>	// for prctl(3)
#include <multiproc_utils.h>	// for print_code(), print_status()
#include <signal.h>	// for psiginfo(3)
#include <signal_utils.h>	// for signal_register_handler_sigaction()

/*
 * This example shows how a child can be notified of the parent death.
 *
 * References:
 * http://www.quora.com/Linux/On-Linux-how-can-I-make-a-process-die-when-its-parent-dies
 */

// count the number of signals we get
static unsigned int counter=0;

static void handler(int sig, siginfo_t *si, void *unused) {
	printf("sighandler: gettid() is %d\n", gettid());
	printf("sighandler: counter is %d\n", counter);
	printf("sighandler: got signal %s\n", strsignal(sig));
	printf("sighandler: si is %p\n", si);
	printf("sighandler: address is: 0x%lx\n", (long) si->si_addr);
	printf("sighandler: psiginfo follows...\n");
	psiginfo(si, "sighandler");
	counter++;
}

int main(int argc, char** argv, char** envp) {
	pid_t child_pid=CHECK_NOT_M1(fork());
	if(child_pid!=0) {
		TRACE("this is the parent");
		CHECK_ZERO(sleep(1));
		TRACE("parent is dying...");
	} else {
		TRACE("this is the child");
		signal_register_handler_sigaction(SIGUSR1, handler, 0);
		CHECK_NOT_M1(prctl(PR_SET_PDEATHSIG, SIGUSR1));
		bool cont=true;
		while(cont) {
			int ret=pause();
			CHECK_ASSERT(ret==-1 && errno==EINTR);
			cont=false;
		}
		TRACE("child is dying...");
	}
	return EXIT_SUCCESS;
}
