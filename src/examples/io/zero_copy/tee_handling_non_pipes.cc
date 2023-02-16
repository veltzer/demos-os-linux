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
#include <sys/types.h>	// for open(2), fstat(2), struct stat
#include <sys/stat.h>	// for open(2), fstat(2), struct stat
#include <fcntl.h>	// for open(2), splice(2), tee(2)
#include <unistd.h>	// for close(2), fstat(2), struct stat, pipe(2)
#include <limits.h>	// for INT_MAX
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This is an improvement of the tee(1) implementation so that it
 * would be able to use non pipe stdin and stdout.
 */

int main(int argc, char** argv, char** envp) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [outfile]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	// parameters
	const char* fileout=argv[1];

	// code
	struct stat stdin_buf, stdout_buf;
	CHECK_NOT_M1(fstat(STDIN_FILENO, &stdin_buf));
	CHECK_NOT_M1(fstat(STDOUT_FILENO, &stdout_buf));
	bool is_pipe_in=S_ISFIFO(stdin_buf.st_mode);
	bool is_pipe_out=S_ISFIFO(stdout_buf.st_mode);
	int real_fdin, real_fdout;
	int pipe_in[2];
	int pipe_out[2];
	if(is_pipe_in) {
		real_fdin=STDIN_FILENO;
	} else {
		CHECK_NOT_M1(pipe(pipe_in));
		real_fdin=pipe_in[0];
	}
	if(is_pipe_out) {
		real_fdout=STDOUT_FILENO;
	} else {
		CHECK_NOT_M1(pipe(pipe_out));
		real_fdout=pipe_out[1];
	}
	int fd=CHECK_NOT_M1(open(fileout, O_WRONLY|O_CREAT|O_TRUNC|O_LARGEFILE, 0666));
	ssize_t ret;
	bool stop_input=false;
	bool stop_output=false;
	do {
		if(!is_pipe_in && !stop_input) {
			ssize_t ret_inner=CHECK_NOT_M1(splice(STDIN_FILENO, NULL, pipe_in[1], NULL, INT_MAX, SPLICE_F_MOVE));
			if(ret_inner==0) {
				stop_input=true;
				CHECK_NOT_M1(close(pipe_in[1]));
			}
		}
		ret=CHECK_NOT_M1(tee(real_fdin, real_fdout, INT_MAX, 0));
		if(!is_pipe_out && !stop_output && ret>0) {
			ssize_t ret_inner=CHECK_NOT_M1(splice(pipe_out[0], NULL, STDOUT_FILENO, NULL, INT_MAX, SPLICE_F_MOVE));
			if(ret_inner==0) {
				stop_output=true;
			}
		}
		ssize_t len=ret;
		while (len>0) {
			// the INT_MAX on the next line could be replaced with 'len'
			len-=CHECK_NOT_M1(splice(real_fdin, NULL, fd, NULL, INT_MAX, SPLICE_F_MOVE));
		}
	} while(ret>0);
	CHECK_NOT_M1(close(fd));
	return EXIT_SUCCESS;
}
