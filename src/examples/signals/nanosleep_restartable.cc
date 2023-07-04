/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for fprintf(3)
#include <unistd.h>	// for sleep(3)
#include <stdlib.h>	// for EXIT_SUCCESS, atoi(3)
#include <err_utils.h>	// for CHECK_ZERO()
#include <assert.h>	// for assert(3)

/*
 * This is an exapmle of how to restart nanosleep(2) even after system call the correct
 * way.
 * The trick is to always sleep the remaining time.
 */

static void handler(int signum) {
	printf("got signal %d...\n", signum);
	//sleep(2);
}

static void print_current_time() {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	printf("now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

int main(int argc, char** argv, char** envp) {
	fprintf(stderr, "signal me with one of the following:\n");
	fprintf(stderr, "[kill -s SIGUSR1 %d]\n", getpid());
	CHECK_NOT_SIGT(signal(SIGUSR1, handler), SIG_ERR);
	struct timespec timeout;
	timeout.tv_sec = 30;
	timeout.tv_nsec = 0;
	print_current_time();
	while(1) {
		int ret=nanosleep(&timeout, &timeout);
		if(ret==-1) {
			assert(errno==EINTR);
			printf("restarting system call\n");
		} else {
			break;
		}
	}
	print_current_time();
	return EXIT_SUCCESS;
}
