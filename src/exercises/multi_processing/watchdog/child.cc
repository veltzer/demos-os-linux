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
#include <sys/types.h>	// for kill(2), SIGUSR1
#include <signal.h>	// for kill(2), SIGUSR1
#include <unistd.h>	// for sleep(3)
#include <stdio.h>	// for fprintf(3), stderr
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, atoi(3)
#include <err_utils.h>	// for CHECK_ZERO()

int main(int argc, char** argv) {
	if (argc < 2) {
		fprintf(stderr, "%s: usage: %s 1 or 2\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	int me=atoi(argv[1]);
	if (me < 0 || me > 2) {
		fprintf(stderr, "I said 1 or 2\n");
		return EXIT_FAILURE;
	}
	int sigme=0;
	switch(me) {
	case 1:
		sigme=SIGUSR1;
		break;
	case 2:
		sigme=SIGUSR2;
		break;
	}
	while(true) {
		CHECK_ZERO(sleep(2));
		CHECK_NOT_M1(kill(getppid(), sigme));
	}
	return EXIT_SUCCESS;
}
