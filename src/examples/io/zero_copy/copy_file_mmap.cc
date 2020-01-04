/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <sys/types.h>	// for open(2), ftruncate(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <unistd.h>	// for ftruncate(2), close(2)
#include <sys/mman.h>	// for mmap(2)
#include <string.h>	// for memcpy(3)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_NOT_VOIDP()

/*
 * This example shows a more efficient copy of file using mmap.
 *
 * NOTES:
 * - The MAP_PRIVATE in the first mapping is because we may not have write permission on the input file.
 * - the O_RDWR for the target file is to enable MAP_SHARED - otherwise mmap fails.
 */

int main(int argc, char** argv, char** envp) {
	if(argc!=3) {
		fprintf(stderr, "%s: usage: %s [infile] [outfile]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	const char* filein=argv[1];
	const char* fileout=argv[2];
	int fd_src=CHECK_NOT_M1(open(filein, O_RDONLY|O_LARGEFILE));
	struct stat statbuf;
	fstat (fd_src, &statbuf);
	size_t len=statbuf.st_size;
	int fd_dst=CHECK_NOT_M1(open(fileout, O_RDWR|O_CREAT|O_LARGEFILE, statbuf.st_mode));
	CHECK_NOT_M1(ftruncate(fd_dst, len));
	void* src=CHECK_NOT_VOIDP(mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd_src, 0), MAP_FAILED);
	void* dst=CHECK_NOT_VOIDP(mmap(NULL, len, PROT_WRITE, MAP_SHARED, fd_dst, 0), MAP_FAILED);
	CHECK_NOT_M1(close(fd_src));
	CHECK_NOT_M1(close(fd_dst));
	memcpy(dst, src, len);
	return EXIT_SUCCESS;
}
