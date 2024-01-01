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
#include <fcntl.h>	// for open(2)
#include <sys/sendfile.h>	// for senffile(2)
#include <unistd.h>	// getpagesize(2)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <limits.h>	// for INT_MAX

/*
 * This is an simple cat(1) using sendfile(2)
 */

void cat(const char* filein) {
	int fdin=CHECK_NOT_M1(open(filein, O_RDONLY | O_LARGEFILE, 0666));
	// this is the main copy loop
	// we go out of the loop because of error or eof
	// >0: would have kept us in the loop
	// =0: that is ok - it is end of file
	// -1: error
	// we need the return value outside the loop
	ssize_t ret;
	do {
		ret=CHECK_NOT_M1(sendfile(STDOUT_FILENO, fdin, NULL, INT_MAX));
	} while(ret>0);
	CHECK_NOT_M1(close(fdin));
}

int main(int argc, char** argv, char** envp) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [infile]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	const char* filein=argv[1];
	cat(filein);
	return EXIT_SUCCESS;
}
