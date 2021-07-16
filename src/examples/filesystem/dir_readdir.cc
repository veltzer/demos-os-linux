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
#include <stdio.h>	// for fprintf(3), stderr
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_ASSERT()
#include <us_helper.h>	// for syscall_readdir(), struct old_linux_dirent
#include <trace_utils.h>// for INFO()

/*
 * This example shows how to read the content of a directory using
 * the readdir(2) system call (old and deprecated way).
 *
 * readdir(2) is a bad API since:
 * - it's transfer data is always large as the largest file.
 * - in every call it transfers EXACTLY one directory entry which
 * requires lots of calls to get very big folders of information.
 * - it does NOT provide lots of needed info (like file type).
 *
 * A great part of this example was shamelessly stolen from the getdents(2)
 * manual page and adjusted for the readdir(2) system call.
 */

int main(int argc, char** argv, char** envp) {
#ifndef __x86_64__
	// command line parsing
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [dir]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: %s /etc\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	// parameters
	const char* dirname=argv[1];
	// code
	int fd=CHECK_NOT_M1(open(dirname, O_RDONLY | O_DIRECTORY));
	struct old_linux_dirent d;
	printf("%8s %8s %12s %20s\n", "i-node#", "d_reclen", "d_off", "d_name");
	while(true) {
		int nread=CHECK_NOT_M1(syscall_readdir(fd, &d, sizeof(d)));
		if(nread==0) {
			break;
		}
		// readdir(2) only gives one result
		CHECK_ASSERT(nread==1);
		printf("%8ld %8d %12ld %20s\n", d.d_ino, d.d_reclen, d.d_off, d.d_name);
	}
#endif	/* __x86_64__ */
	return EXIT_SUCCESS;
}
