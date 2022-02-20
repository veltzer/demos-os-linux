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
#include <signal.h>	// for sigqueue(2), SIGRTMIN, SIGRTMIN
#include <stdio.h>	// for fprintf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, atoi(3)
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This is a general command line utility to send signals via
 * the sigqueue(2) interface.
 */

int main(int argc, char** argv, char** envp) {
	if (argc!=4) {
		fprintf(stderr, "%s: usage: %s [pid] [sig] [value]\n", argv[0], argv[0]);
		fprintf(stderr, "Info for real time signals follows:\n");
		fprintf(stderr, "SIGRTMIN is %d\n", SIGRTMIN);
		fprintf(stderr, "SIGRTMAX is %d\n", SIGRTMAX);
		return EXIT_FAILURE;
	}
	int pid=atoi(argv[1]);
	int sig=atoi(argv[2]);
	int val=atoi(argv[3]);
	union sigval sval;
	sval.sival_int=val;
	CHECK_NOT_M1(sigqueue(pid, sig, sval));
	return EXIT_SUCCESS;
}
