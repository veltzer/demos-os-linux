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
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, atoi(3)
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2), splice(2), tee(2)
#include <limits.h>	// for INT_MAX
#include <unistd.h>	// for close(2)
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This example demonstrates how to implement tee(1) using tee(2)
 * by using zero copy as much as possible.
 * This version does not spin as the bad version does.
 *
 * Notes:
 * - this tee(1) is very limited. Both stdin and stdout HAVE to be pipes.
 * This means that if you don't want to see errors you should run this program
 * like this:
 * cat /etc/passwd | [this program] [outfile] | cat
 * both pipes are neccessary
 *
 * References:
 * http://www.scribd.com/doc/4006475/Splice-Tee-VMsplice-zero-copy-in-Linux
 */

int main(int argc, char** argv) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [outfile]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	const char* fileout=argv[1];
	int fd=CHECK_NOT_M1(open(fileout, O_WRONLY|O_CREAT|O_TRUNC|O_LARGEFILE, 0666));
	ssize_t ret;
	do {
		ret=CHECK_NOT_M1(tee(STDIN_FILENO, STDOUT_FILENO, INT_MAX, 0));
		ssize_t len=ret;
		while (len>0) {
			// the INT_MAX on the next line could be replaced by 'len'
			len-=CHECK_NOT_M1(splice(STDIN_FILENO, NULL, fd, NULL, INT_MAX, SPLICE_F_MOVE));
		}
	} while(ret>0);
	CHECK_NOT_M1(close(fd));
	return EXIT_SUCCESS;
}
