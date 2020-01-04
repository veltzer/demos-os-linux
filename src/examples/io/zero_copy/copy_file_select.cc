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
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <unistd.h>	// for close(2), getpagesize()
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <Selector.hh>	// for Selector
#include <CircularPipe.hh>	// for CircularPipe

/*
 * This example shows how to copy a copy file program using the select system
 * call.
 *
 * The idea is to have a buffer of a certain size. When there is room in the buffer
 * keep calling read. When there is data in the buffer keep calling write. Handle
 * the buffer in a circular way. No need for any synchronization in this implementation
 * since only one thread is doing all the work.
 *
 * The advantage of this implementation over a simple read and write implementation is that
 * requests for read(2) and write(2) could be issued by this app * at the same time *.
 * This is especially true for read(2) calls which are synchroneous and blocking. In the
 * simple read+write implementation a write will never be issued while a read is runnig
 * which in this implementation we could issue a write while a read is going on. This is
 * especially useful when you are copying files between two different hard disks.
 */

void copy_file(const char* filein, const char* fileout, const unsigned int bufsize) {
	CircularPipe p(bufsize);
	int fdin=CHECK_NOT_M1(open(filein, O_RDONLY|O_LARGEFILE, 0666));
	int fdout=CHECK_NOT_M1(open(fileout, O_WRONLY|O_CREAT|O_TRUNC|O_LARGEFILE, 0666));
	Selector s;
	bool eof=false;
	do {
		s.null();
		if(p.canPush() && !eof) {
			s.addReadFd(fdin);
		}
		if(p.canPull()) {
			s.addWriteFd(fdout);
		}
		s.doSelect();
		// It doesn't matter who we handle first: if we handle the read first then
		// we will have more to write. If we handle the write first then we will
		// have more room to read into.
		if (s.isReadActive(fdin)) {
			if(p.push(fdin)) {
				eof=true;
			}
		}
		if (s.isWriteActive(fdout)) {
			p.pull(fdout);
		}
	} while (!eof || p.canPull());
	CHECK_NOT_M1(close(fdin));
	CHECK_NOT_M1(close(fdout));
}

int main(int argc, char** argv, char** envp) {
	if(argc!=4) {
		fprintf(stderr, "%s: usage: %s [infile] [outfile] [num_pages]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	const char* filein=argv[1];
	const char* fileout=argv[2];
	const unsigned int num_pages=atoi(argv[3]);
	copy_file(filein, fileout, num_pages*getpagesize());
	return EXIT_SUCCESS;
}
