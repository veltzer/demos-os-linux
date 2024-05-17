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
#include <syslog.h>	// for openlog(3), syslog(3), closelog(3), setlogmask(3)
#include <unistd.h>	// for getpid(2), getppid(2), setsid(2), fork(2),
			// chdir(2), close(2), getdtablesize(2), dup2(2), sleep(3), isatty(3), ttyname(3)
#include <stdlib.h>	// for EXIT_SUCCESS, exit(3)
#include <stdio.h>	// for printf(3), fopen(3), fclose(3), fprintf(3), stdin: object, stderr:object
#include <string.h>	// for strdup(3)
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ZERO(), CHECK_NOT_NULL_CHARP(), CHECK_ASSERT(), CHECK_NOT_NULL_FILEP(), CHECK_POSITIVE()

/*
 * This is an example of writing a daemon by yourself without
 * using the glibc daemon(3) support function or any daemonization
 * library.
 *
 * NOTES:
 * - On some Linux systems (notably newer ones using systemd(1)) the daemon
 * may not get adopted by process pid=1 but rather by a lower reaper in charge
 * of reaping all processes by that login session. This is fine.
 */

void my_daemon() {
	// let parent die and get adopted by init(1)...
	if(CHECK_NOT_M1(fork())) {
		exit(EXIT_SUCCESS);
	}
	// become session leader...
	CHECK_NOT_M1(setsid());
	// avoid 'unable to unmount' errors...
	CHECK_NOT_M1(chdir("/"));
	// close all open files.
	// We check the return value of close(2) very carefully since
	// some of the entries in our file descriptor table may not
	// refer to open files and so we skip them.
	// real errors will cause us to halt.
	int fdTableSize = getdtablesize();
	for(int i=0; i<fdTableSize; i++) {
		int ret=close(i);
		if (ret==-1) {
			int myerrno=errno;
			CHECK_ASSERT(myerrno==EBADF);
		}
	}
	// open standard streams to the null device
	// in case someone writes to them...
	// according to open(2) spec it will return the lowest
	// possible file descriptor number (which is 0 in our case)
	CHECK_ZERO(open("/dev/null", O_RDWR));
	CHECK_NOT_M1(dup2(0,1));
	CHECK_NOT_M1(dup2(0,2));
}

int main(int, char** argv) {
	printf("before daemon(3), pid is [%d], ppid is [%d]\n", getpid(), getppid());
	printf("after the console returns the process will continue disconnected\n");
	printf("from the terminal as child of pid 1 or some lower reaper\n");
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
	// turn into a daemon
	my_daemon();
	// from this point prints do not get anywhere (/dev/null redirection).
	// so we use the terminal we collected before instead.
	// note that the right thing to do it to use syslog(3) instead.
	FILE* newout;
	if (myttyname) {
		newout=CHECK_NOT_NULL_FILEP(fopen(myttyname,"w"));
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
