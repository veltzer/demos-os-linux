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
#include <stdio.h>	// for fprintf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, atoi(3)
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2), splice(2), tee(2)
#include <limits.h>	// for INT_MAX
#include <unistd.h>	// for close(2)
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This example is an attempt to improve the basic tee.cc solution by
 * not blocking on the splice(2) system call.
 *
 * The problem with this solution is that if we get a short splice then
 * some data is left in the input stream and this data could be tee(2)ed
 * again. This is because tee(2) does not remember what data it has
 * already tee(2)ed out and just tee(2)s anything it finds.
 *
 * TODO:
 * - show indeed that this is a bug. Produce a real scenario where this produces
 * the wrong output and show that it is wrong.
 */

int main(int argc, char** argv, char** envp) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [outfile]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	const char* fileout=argv[1];
	int fd=CHECK_NOT_M1(open(fileout, O_WRONLY|O_CREAT|O_TRUNC|O_LARGEFILE, 0666));
	ssize_t splice_pos=0;
	ssize_t tee_pos=0;
	ssize_t ret;
	do {
		ret=CHECK_NOT_M1(tee(STDIN_FILENO, STDOUT_FILENO, INT_MAX, 0));
		tee_pos+=ret;
		splice_pos+=CHECK_NOT_M1(splice(STDIN_FILENO, NULL, fd, NULL, tee_pos-splice_pos, SPLICE_F_MOVE | SPLICE_F_NONBLOCK));
	} while(ret>0);
	while(splice_pos<tee_pos) {
		splice_pos+=CHECK_NOT_M1(splice(STDIN_FILENO, NULL, fd, NULL, tee_pos-splice_pos, SPLICE_F_MOVE));
	}
	CHECK_NOT_M1(close(fd));
	return EXIT_SUCCESS;
}
