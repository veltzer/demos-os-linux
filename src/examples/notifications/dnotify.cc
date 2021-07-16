/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <sys/inotify.h>// for inotify_init(2), inotify_add_watch(2)
#include <stdio.h>	// for printf(3)
#include <unistd.h>	// for read(2), close(2)
#include <assert.h>	// for assert(3)
#include <errno.h>	// for errno(3)
#include <sys/types.h>	// for getpid(2)
#include <stdlib.h>	// for EXIT_FAILURE, EXIT_SUCCESS
#include <signal.h>	// for siginfo_t
#include <string.h>	// for strcmp(3)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ASSERT()
#include <fcntl.h>	// for DN_* constants

/*
 * Demonstrate the use of the (obsolete) dnotify feature to obtain directory
 * change notifications. (Modern programs should use inotify instead of
 * dnotify. The inotify API is available in Linux 2.6.13 and later.)
 * usage is as shown in usageError() below. An example is the following:
 * dnotify dir1:a xyz/dir2:acdM
 * See also demo_inotify.c.
 *
 * This example was stolen shamelessly from Michael Kerrisk (author of
 * the linux manual pages).
 *
 * TODO:
 * - show how to get the info about what happened in the signal handler.
 * - make the command line parsing and printing of dnotify events nicer.
 * (print the events symbolicaly and not numericaly...)
 * - print ALL the fields of siginfo, do a helper method to do
 * that if I don't already have one.
 * - see if we get some info from the kernel
 * if not state that we dont.
 * if we do the print it nicely.
 * - clean up that example.
 * - better command line parsing.
 */

/* Print (optional) message and usage info, then exit */
static void usageError(const char *progName, const char *msg) {
	if (msg != NULL)
		fprintf(stderr, "%s", msg);
	fprintf(stderr, "%s: usage: %s directory:[events]...\n", progName, progName);
	fprintf(stderr, "%s: Events are:\n", progName);
	fprintf(stderr, "%s: a - access; A - attrib; c - create; d - delete\n", progName);
	fprintf(stderr, "%s: m - modify; r - rename; M - multishot\n", progName);
	fprintf(stderr, "%s: (default is all events)\n", progName);
	exit(EXIT_FAILURE);
}

static void handler(int sig, siginfo_t *si, void *ucontext) {
	printf("got event on descriptor %d\n", si->si_fd);
	printf("got code %d\n", si->si_code);
}

int main(int argc, char** argv, char** envp) {
	struct sigaction sa;
	int events, fnum;
	const int NOTIFY_SIG = SIGRTMIN;
	char *p;
	if (argc < 2 || strcmp(argv[1], "--help") == 0)
		usageError(argv[0], NULL);
	/* Establish handler for notification signal */
	sa.sa_sigaction = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	// this will ensure that we don't lose messages
	CHECK_NOT_M1(sigaction(NOTIFY_SIG, &sa, NULL));
	for(fnum = 1; fnum < argc; fnum++) {
		p = strchr(argv[fnum], ':');	/* Look for optional ':' */
		if (p == NULL) {/* Default is all events + multishot */
			events = DN_ACCESS | DN_ATTRIB | DN_CREATE | DN_DELETE | DN_MODIFY | DN_RENAME | DN_MULTISHOT;
		} else {/* ':' present, parse event chars */
			*p = '\0';	/* Terminates directory component */
			events = 0;
			for(p++; *p != '\0'; p++) {
				switch(*p) {
				case 'a': events |= DN_ACCESS; break;
				case 'A': events |= DN_ATTRIB; break;
				case 'c': events |= DN_CREATE; break;
				case 'd': events |= DN_DELETE; break;
				case 'm': events |= DN_MODIFY; break;
				case 'r': events |= DN_RENAME; break;
				case 'M': events |= DN_MULTISHOT; break;
				default:
					usageError(argv[0], "Bad event character\n");
					break;
				}
			}
		}
		/* Obtain a file descriptor for the directory to be monitored */
		int fd=CHECK_NOT_M1(open(argv[fnum], O_RDONLY));
		printf("opened '%s' as file descriptor %d\n", argv[fnum], fd);
		/* Use alternate signal instead of SIGIO for dnotify events */
		CHECK_NOT_M1(fcntl(fd, F_SETSIG, NOTIFY_SIG));
		/* Enable directory change notifications */
		CHECK_NOT_M1(fcntl(fd, F_NOTIFY, events));
		printf("events: %o\n", (unsigned int) events);
	}
	// a non busy busy wait loop...
	while(true) {
		int ret=pause();
		CHECK_ASSERT(ret==-1 && errno==EINTR);
	}
	return EXIT_SUCCESS;
}
