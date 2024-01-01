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
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h>	// for fprintf(3), stderr
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <unistd.h>	// for read(2), close(2)
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This example shows that when you try to directly read(2) from a directory
 * you get an error which indicates that this is a directory you are trying
 * to read from.
 *
 * This example means that the only way to read from a directory is using:
 * readdir(2), getdents(2), readdir(3)
 *
 * NOTES:
 * - note that the open(2)/close(2) calls do succeed for directories.
 */

int main(int argc, char** argv, char** envp) {
	// command line parsing
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [dir]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: %s /etc\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	// parameters
	const char* dirname=argv[1];
	int fd=CHECK_NOT_M1(open(dirname, O_RDONLY));
	char buf[1024];
	CHECK_NOT_M1(read(fd, buf, sizeof(buf)));
	CHECK_NOT_M1(close(fd));
	return EXIT_SUCCESS;
}
