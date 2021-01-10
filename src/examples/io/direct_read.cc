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
#include <stdio.h>	// for printf(3), fprintf(3), stderr:struct
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, aligned_alloc(3), malloc(3)
#include <strings.h>	// for bzero(3)
#include <sys/types.h>	// for open(2), fstat(2)
#include <sys/stat.h>	// for open(2), fstat(2)
#include <fcntl.h>	// for open(2)
#include <sys/ioctl.h>	// for ioctl(2)
#include <unistd.h>	// for fstat(2)
#include <linux/fs.h>	// for BLKSSZGET
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_INT()
#include <multiproc_utils.h>	// for my_system()

/*
 * This example explores the performance impact of the O_DIRECT
 * flag when working with a file.
 * This example also shows (look at the source code) how you can
 * create a big file filled with random data using /dev/urandom
 * and the dd(1) command...
 * This example proves that you cannot use regular malloc(3) to
 * do direct IO since you need an address which is a multiple of
 * the underlying used block size and a size which is a multiple
 * of the same. play around with the command line parameters to
 * see that.
 *
 * TODO:
 * - show the difference in performance between doing O_DIRECT and
 * not (maybe in another example and not this one?!?).
 * - use the BLKSSZGET ioctl(2) (see the code below) to get the block
 * size as well as the fstat(2) that is currently used.
 *
 * REFERENCES:
 * http://www.quora.com/Linux/How-can-I-bypass-the-OS-buffering-during-I-O-in-Linux
 * http://www.quora.com/Why-does-O_DIRECT-require-I-O-to-be-512-byte-aligned
 */

int main(int argc, char** argv, char** envp) {
	if(argc!=4) {
		fprintf(stderr, "%s: usage: %s [use_direct] [use_malloc] [size]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example: for one MB use:\n", argv[0]);
		fprintf(stderr, "%s: example: %s 1 0 1048576\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	const unsigned int use_direct=atoi(argv[1]);
	const unsigned int use_malloc=atoi(argv[2]);
	const unsigned int size=atoi(argv[3]);
	const char* filename="/tmp/bigfile";
	printf("creating the big file...\n");
	my_system("dd if=/dev/urandom of=%s count=%d", filename, size/512);
	int flags=O_RDONLY;
	if(use_direct) {
		flags|=O_DIRECT;
	}
	int fd=CHECK_NOT_M1(open(filename, flags));
	// find out the block size
	blksize_t block_size1;
	CHECK_NOT_M1(ioctl(fd, BLKSSZGET, &block_size1));
	printf("block_size1=%ld\n", block_size1);
	struct stat mystat;
	CHECK_NOT_M1(fstat(fd, &mystat));
	blksize_t block_size=mystat.st_blksize;
	char* p;
	if(use_malloc) {
		p=(char*)malloc(size);
	} else {
		p=(char*)aligned_alloc(block_size, size);
	}
	if(((unsigned long)p)%block_size!=0) {
		printf("p (%p) is not a multiple of block_size (%ld) !!!...\n", p, block_size);
	} else {
		printf("p (%p) is a multiple of block_size (%ld)...\n", p, block_size);
	}
	bzero(p, size);
	CHECK_INT(read(fd, p, size), size);
	CHECK_NOT_M1(close(fd));
	return EXIT_SUCCESS;
}
