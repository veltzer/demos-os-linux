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
#include <stdio.h>	// for stderr, fprintf(3), printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <sys/types.h>	// for open(2), O_WRONLY
#include <sys/stat.h>	// for open(2), O_WRONLY, futimens(2)
#include <fcntl.h>	// for open(2), O_WRONLY, futimens(2)
#include <err_utils.h>	// for CHECK_NOT_M1(), error_get_by_val()

/*
 * This example shows an example of how to implement the touch(1)
 * command line utility using the
 * utime(2)
 * utimes(2)
 * futimes(2)
 * futimens(2)
 * futimesat(2)
 * utimensat(2)
 * family of systems call.
 *
 * Notes:
 * - We use utimensat(2)/futimens(2) because they have better precision than
 * the old utime(2)/utime(2) API and allow to use a file descriptor and not
 * just a file name.
 */

int main(int argc, char** argv, char** envp) {
	// command line parsing
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [file]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: %s /tmp/newfile\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: %s /tmp/existingfile\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	// parameters
	const char* filename=argv[1];
	// code
	int fd=open(filename, O_WRONLY);
	if(fd==-1) {
		int err=errno;
		if(err==ENOENT) {
			fd=CHECK_NOT_M1(creat(filename, 0666));
		} else {
			CHECK_NOT_M1(fd);
		}
	}
	// now we have fd and we can update the times
	CHECK_NOT_M1(futimens(fd, NULL));
	printf("file [%s] touched...\n", filename);
	return EXIT_SUCCESS;
}
