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
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, atoi(3)
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <sys/sendfile.h>	// for senffile(2)
#include <unistd.h>	// getpagesize(2)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <limits.h>	// for INT_MAX

/*
 * This demos shows how to use sendfile(2) to avoid copy to/from user space.
 * The demo itself is just a simple version of cp(1)
 *
 * It seems that this demo doesn't run properly on 2.6.32 and should run
 * on 2.6.33 or later.
 * On 2.6.38 it works well.
 *
 * Notes:
 * - we send the data page by page and call sendfile(2) many times. This could
 * be avoided using fstat(2) after the open of the input file and sending the
 * entire content at once. On the other hand this could cause performance issues.
 * This needs to be checked. In any case the buffer of a single page size
 * as is used here seems too small for good performace on todays systems.
 */

void copy_file(const char* filein, const char* fileout) {
	int fdin=CHECK_NOT_M1(open(filein, O_RDONLY | O_LARGEFILE, 0666));
	int fdout=CHECK_NOT_M1(open(fileout, O_WRONLY|O_CREAT|O_TRUNC | O_LARGEFILE, 0666));
	// this is the main copy loop
	// we go out of the loop because of error or eof
	// >0: would have kept us in the loop
	// =0: that is ok - it is end of file
	// -1: error
	// we need the return value outside the loop
	ssize_t ret;
	do {
		ret=CHECK_NOT_M1(sendfile(fdout, fdin, NULL, INT_MAX));
	} while(ret>0);
	CHECK_NOT_M1(close(fdin));
	CHECK_NOT_M1(close(fdout));
}

int main(int argc, char** argv, char** envp) {
	if(argc!=3) {
		fprintf(stderr, "%s: usage: %s [infile] [outfile]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	const char* filein=argv[1];
	const char* fileout=argv[2];
	copy_file(filein, fileout);
	return EXIT_SUCCESS;
}
