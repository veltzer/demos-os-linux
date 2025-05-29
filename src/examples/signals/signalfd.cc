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
#include <sys/signalfd.h>	// for signalfd(2)
#include <signal.h>	// for sigprocmask(2)
#include <unistd.h>	// for read(2), getpid(2)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <stdio.h>	// for printf(3)
#include <sys/types.h>	// for getpid(2)
#include <err_utils.h>	// for CHECK_INT(), CHECK_NOT_M1()

/*
 * This demo of signalfd was stolen shamelessly from the signalfd(2) manpage.
 */

int main() {
	printf("send a signal to me using [kill -s SIGINT %d]\n", getpid());
	printf("signals are [SIGINT, SIGQUIT, SIGUSR1]\n");
	printf("SIGINT(2): keyboard interrupt, will do nothing\n");
	printf("SIGQUIT(3): will quit the app\n");
	printf("SIGUSR1(30,10,16): will print a message\n");
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGQUIT);
	sigaddset(&mask, SIGUSR1);
	/* Block signals so that they aren't handled according to their default dispositions */
	CHECK_NOT_M1(sigprocmask(SIG_BLOCK, &mask, NULL));
	int sfd=CHECK_NOT_M1(signalfd(-1, &mask, 0));
	while(true) {
		struct signalfd_siginfo fdsi;
		CHECK_INT(read(sfd, &fdsi, sizeof(struct signalfd_siginfo)), sizeof(struct signalfd_siginfo));
		switch(fdsi.ssi_signo) {
		case SIGINT:
			printf("Got SIGINT\n");
			break;
		case SIGQUIT:
			printf("Got SIGQUIT\n");
			return EXIT_SUCCESS;
			break;
		case SIGUSR1:
			printf("Got SIGUSR1\n");
			break;
			/*
			 * default:
			 * printf("ERROR: Read unexpected signal\n");
			 * break;
			 */
		}
	}
	return EXIT_SUCCESS;
}
