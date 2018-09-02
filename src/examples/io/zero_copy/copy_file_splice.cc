/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for exit(3), EXIT_SUCCESS, EXIT_FAILURE, atoi(3)
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2), splice(2), fcntl(2)
#include <unistd.h>	// getpagesize(2), fcntl(2)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <limits.h>	// for INT_MAX

/*
 * This demos shows how to use splice(2) to avoid copy to/from user space.
 * The demo itself is just a simple version of cp(1)
 */

void copy_file(const char* filein, const char* fileout, const bool setbufsize, const size_t buf_size) {
	// lets create a pipe
	const unsigned int splice_flags=SPLICE_F_MOVE | SPLICE_F_MORE;
	//const unsigned int splice_flags=SPLICE_F_MOVE;
	int pipe_fds[2];
	CHECK_NOT_M1(pipe(pipe_fds));
	// lets get the pipe size
	// size_t pipe_size=CHECK_NOT_M1(fcntl(pipe_fds[0],F_GETPIPE_SZ));
	// printf("pipe_size is %d\n",pipe_size);
	// lets make the pipe big for better performance
	size_t size;
	if(setbufsize) {
		CHECK_NOT_M1(fcntl(pipe_fds[0], F_SETPIPE_SZ, buf_size));
		size=buf_size;
	} else {
		size=INT_MAX;
		//size=getpagesize();
	}
	int fdin=CHECK_NOT_M1(open(filein, O_RDONLY|O_LARGEFILE, 0666));
	int fdout=CHECK_NOT_M1(open(fileout, O_WRONLY|O_CREAT|O_TRUNC|O_LARGEFILE, 0666));
	ssize_t ret;
	do {
		ret=CHECK_NOT_M1(splice(fdin, 0, pipe_fds[1], 0, size, splice_flags));
		ssize_t len=ret;
		// now splice everything we got...
		while(len>0) {
			len-=CHECK_NOT_M1(splice(pipe_fds[0], 0, fdout, 0, size, splice_flags));
		}
	} while(ret>0);
	CHECK_NOT_M1(close(pipe_fds[0]));
	CHECK_NOT_M1(close(pipe_fds[1]));
	CHECK_NOT_M1(close(fdin));
	CHECK_NOT_M1(close(fdout));
}

int main(int argc, char** argv, char** envp) {
	if(argc!=5) {
		fprintf(stderr, "%s: usage: %s [infile] [outfile] [setbufsize] [num_pages]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: try to use setbufsize=1 and num_pages=8 to match what is going on in the official cp(1)\n", argv[0]);
		fprintf(stderr, "%s: which was derived by strac(1)ing cp(1)\n", argv[0]);
		return EXIT_FAILURE;
	}
	const char* filein=argv[1];
	const char* fileout=argv[2];
	bool setbufsize=atoi(argv[3]);
	const unsigned int num_pages=atoi(argv[4]);
	copy_file(filein, fileout, setbufsize, num_pages*getpagesize());
	return EXIT_SUCCESS;
}
