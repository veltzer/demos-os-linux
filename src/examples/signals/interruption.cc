/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2024 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for scanf(3), printf(3), fflush(3)
#include <sys/types.h>	// for getpid(2)
#include <unistd.h>	// for getpid(2)
#include <errno.h>	// for errno(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// CHECK_NOT_M1()
#include <trace_utils.h>// for TRACE()
#include <signal_utils.h>	// for signal_register_handler_sigaction()

/*
 * This is an example of a calculator that you can break out of.
 * Notice that this example is written as a single thread and you can
 * still break out at any time from this calculator.
 * The idea is to setup a signal handler as interrupting syscalls
 * (in this case we use SIGUSR1). When that signal is received
 * Take heed to look at errno RIGHT AFTER the scanf fails since
 * any call to other APIs may change it's value and so lose the
 * original value you want to look at.
 *
 * Note:
 * We must register the signal handler for SIGUSR1 since if we
 * do not we will use the default signal handler for SIGUSR1
 * which will kill our program and we do not want that behaviour.
 */

static bool broken=false;
static bool actually_break=false;

static void handler(int sig, siginfo_t *, void *) {
	TRACE("start: sig is %d", sig);
	TRACE("end");
}

int main() {
	// make sure we break out on receiving the SIGUSR1 signal...
	if(actually_break) {
		signal_register_handler_sigaction(SIGUSR1, handler, 0);
	} else {
		signal_register_handler_sigaction(SIGUSR1, handler, SA_RESTART);
	}
	int ret;
	printf("signal me with [kill -s SIGUSR1 %d]\n", getpid());
	bool matchingError=false;
	while(true) {
		printf("enter number a: ");
		fflush(stdout);
		int a;
		if(broken) {
			if(actually_break) {
				break;
			}
		}
		ret=scanf("%d", &a);
		if(ret==EOF && errno==EINTR) {
			printf("interrupted, shutting down\n");
			broken=true;
			break;
		}
		if(ret!=1) {
			matchingError=true;
			break;
		}
		printf("enter number b: ");
		fflush(stdout);
		int b;
		ret=scanf("%d", &b);
		if(ret==EOF && errno==EINTR) {
			printf("interrupted, shutting down\n");
			broken=true;
			break;
		}
		if(ret!=1) {
			matchingError=true;
			break;
		}
		printf("a+b is %d\n", a+b);
		fflush(stdout);
	}
	if(broken) {
		printf("doing some cleanup code at end of program in case of break\n");
	}
	if(matchingError) {
		printf("doing some cleanup code at end of program in case of EOF or parse error\n");
	}
	return EXIT_SUCCESS;
}
