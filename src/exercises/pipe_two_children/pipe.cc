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
#include <sys/types.h>	// for wait(2)
#include <sys/wait.h>	// for wait(2)
#include <unistd.h>	// for close(2), dup(2), execl(3), fork(2)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This is the first child
 * It closes it's own standrad output (which it inherited from the parent - usually
 * the console where the parent was run).
 * It then closes the side of the pipe that it is not going to use (the read side [0]).
 * This is done so that the other side will get SIGPIPE and eof when all is done.
 * It then duplicates the side of the pipe that it will use (the write side [1]) to its
 * standard output (that it will give to it's children in case of exec).
 * It executes ls which prints to the pipes write side.
 * When ls dies this process dies.
 * The line after execl doesn't get executed...
 */

void doChildOne(int* fd) {
	// close standard output
	CHECK_NOT_M1(close(STDOUT_FILENO));
	// close the read end of the pipe
	CHECK_NOT_M1(close(fd[0]));
	// setup fd 1 to be correct
	CHECK_NOT_M1(dup2(fd[1], STDOUT_FILENO));
	// close the unneeded file descriptor
	CHECK_NOT_M1(close(fd[1]));
	// execute ls -l
	CHECK_NOT_M1(execl("/bin/ls", "/bin/ls", "-l", NULL));
}

/*
 * Very similar to the above...
 */

void doChildTwo(int* fd) {
	// close standard input
	CHECK_NOT_M1(close(STDIN_FILENO));
	// close the write end of the pipe
	CHECK_NOT_M1(close(fd[1]));
	// setup fd 1 to be correct
	CHECK_NOT_M1(dup2(fd[0], STDIN_FILENO));
	// close the unneeded file descriptor
	CHECK_NOT_M1(close(fd[0]));
	// execute ls -l
	CHECK_NOT_M1(execl("/usr/bin/wc", "/usr/bin/wc", "-l", NULL));
}

int main(int argc, char** argv, char** envp) {
	int fd[2];
	CHECK_NOT_M1(pipe(fd));
	// child one
	int pid1=CHECK_NOT_M1(fork());
	if(pid1==0) {
		doChildOne(fd);
	}
	// child two
	int pid2=CHECK_NOT_M1(fork());
	if(pid2==0) {
		doChildTwo(fd);
	}
	// close the pipe at the parent
	// we cannot close before the fork or the children will not be able to use this
	// pipe...
	CHECK_NOT_M1(close(fd[0]));
	CHECK_NOT_M1(close(fd[1]));
	// wait for both children to die...
	CHECK_NOT_M1(wait(NULL));
	CHECK_NOT_M1(wait(NULL));
	return EXIT_SUCCESS;
}
