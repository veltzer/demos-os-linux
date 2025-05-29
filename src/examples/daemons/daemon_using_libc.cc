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
#include <syslog.h>	// for openlog(3), syslog(3), closelog(3), fileno(3)
#include <unistd.h>	// for daemon(3), getpid(2), getppid(2), sleep(3), ttyname(3), isatty(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <stdio.h>	// for printf(3), fprintf(3), stdin:object, stderr:object, fopen(3), fclose(3)
#include <string.h>	// for strdup(3)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_POSITIVE(), CHECK_NOT_NULL_FILEP(), CHECK_NOT_NULL_CHARP()

/*
 * This is an example of using the daemon(3) function of glibc.
 *
 * NOTES:
 * - On some Linux systems (notably newer ones using systemd(1)) the daemon
 * may not get adopted by process pid=1 but rather by a lower reaper in charge
 * of reaping all processes by that login session. This is fine.
 */

int main(int, char** argv) {
	printf("before daemon(3), pid is [%d], ppid is [%d]\n", getpid(), getppid());
	printf("after the console returns the process will continue disconnected\n");
	printf("from the terminal and will be adopted by pid=1 or some lower reaper\n");
	printf("use [ps -f -p %d] to see it's data...\n", getpid());
	printf("use [tail -f /var/log/syslog] to see the logs of this process.\n");
	// save the terminal name for later
	int filedes=fileno(stdin);
	const char* myttyname;
	// if the file descriptor is a terminal then lets get it's name
	// we duplicate the string since it may be overwritten.
	if (isatty(filedes)) {
		myttyname=CHECK_NOT_NULL_CHARP(ttyname(filedes));
		myttyname=strdup(myttyname);
	} else {
		myttyname=NULL;
	}
	if (myttyname) {
		fprintf(stderr, "it is a terminal with name [%s]\n", myttyname);
	} else {
		fprintf(stderr, "it is not a terminal\n");
	}
	// turn into a daemon using the standard C library
	CHECK_NOT_M1(daemon(0, 0));
	// from this point prints do not get anywhere (/dev/null redirection).
	// so we use the terminal we collected before instead.
	// note that the right thing to do it to use syslog(3) instead.
	FILE* newout;
	if (myttyname) {
		newout=CHECK_NOT_NULL_FILEP(fopen(myttyname, "w"));
		CHECK_POSITIVE(fprintf(newout, "after daemon(3), pid is [%d], ppid is [%d]\n", getpid(), getppid()));
	}
	// demo of how to use syslog
	openlog(argv[0], LOG_PID, LOG_USER);
	syslog(LOG_ERR, "after daemon(3), pid is [%d], ppid is [%d]\n", getpid(), getppid());
	sleep(10);
	syslog(LOG_ERR, "daemon dying\n");
	if (myttyname) {
		CHECK_POSITIVE(fprintf(newout, "daemon dying\n"));
	}
	closelog();
	if (myttyname) {
		CHECK_ZERO_ERRNO(fclose(newout));
	}
	return EXIT_SUCCESS;
}
