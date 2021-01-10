/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
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
 * This example is bad as it spins when there is no input.
 * It could be easily fixed by removing the SPLICE_F_NONBLOCK flag,
 * but then it could also be coded in a more elegant way. See tee.cc
 * for an example of this...
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

int main(int argc, char** argv, char** envp) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [outfile]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	const char* fileout=argv[1];
	int fd=CHECK_NOT_M1(open(fileout, O_WRONLY|O_CREAT|O_TRUNC|O_LARGEFILE, 0666));
	do {
		ssize_t len=tee(STDIN_FILENO, STDOUT_FILENO, INT_MAX, SPLICE_F_NONBLOCK);
		if(len==-1) {
			if(errno==EAGAIN) {
				continue;
			}
			CHECK_NOT_M1(len);
		} else {
			if(len==0) {
				break;
			}
			while (len>0) {
				len-=CHECK_NOT_M1(splice(STDIN_FILENO, NULL, fd, NULL, len, SPLICE_F_MOVE));
			}
		}
	} while(true);
	CHECK_NOT_M1(close(fd));
	return EXIT_SUCCESS;
}
