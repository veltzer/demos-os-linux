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
#include <stdio.h>	// for fprintf(3)
#include <unistd.h>	// for sleep(3)
#include <stdlib.h>	// for EXIT_SUCCESS, atoi(3)
#include <err_utils.h>	// for CHECK_ZERO()

/*
 * This is a simple example of calling sleep(3).
 * The idea is to demonstrate that sleep is no longer made out of
 * signal handling functions. but rather is just made up of a single
 * call to nanosleep(2).
 * strace this executable to see exactly what happens.
 *
 * Here is the output:
 * rt_sigprocmask(SIG_BLOCK, [CHLD], [], 8)=0
 * rt_sigaction(SIGCHLD, NULL, {SIG_DFL, [], 0}, 8)=0
 * rt_sigprocmask(SIG_SETMASK, [], NULL, 8)=0
 * nanosleep({5, 0}, 0xbf9db174)=0
 *
 * TODO:
 * - make this program strace itself...
 */

int main(int argc, char** argv) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [seconds]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	int seconds=atoi(argv[1]);
	CHECK_ZERO(sleep(seconds));
	return EXIT_SUCCESS;
}
