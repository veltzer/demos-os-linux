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
#include <sys/types.h>	// for waitpid(2), open(2)
#include <sys/wait.h>	// for waitpid(2)
#include <unistd.h>	// for close(2), dup(2), execl(3), fork(2), pathconf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This is the controller program. It launches the uncontrolled program
 * and determines where it's log will go to, does rotation of logs while
 * the uncontrolled is running etc... All of this WITHOUT copyting the logs,
 * EVER, and without missing a single log message...
 * It can also be used as a watchdog...
 */

void runUncontrolled(int* fd) {
	const char* exe="src/exercises/pipe_stdout_logger/uncontrolled.elf";
	// close standard output
	CHECK_NOT_M1(close(STDOUT_FILENO));
	// close the read end of the pipe
	CHECK_NOT_M1(close(fd[0]));
	// setup fd 1 to be correct
	CHECK_NOT_M1(dup2(fd[1], STDOUT_FILENO));
	// execute the uncontrolled process
	CHECK_NOT_M1(execl(exe, exe, NULL));
}

/*
 * Very similar to the above...
 */

static int logfile_counter=0;
int getnextlogfile() {
	unsigned int filename_length=CHECK_NOT_M1(pathconf("/tmp", _PC_NAME_MAX));
	char* logfilename=new char[filename_length];
	snprintf(logfilename, filename_length, "/tmp/log%d.txt", logfile_counter);
	int logfd=CHECK_NOT_M1(open(logfilename, O_WRONLY|O_CREAT|O_TRUNC, 0666));
	delete[] logfilename;
	logfile_counter++;
	return logfd;
}

void runLogger(int* fd) {
	// close standard input
	CHECK_NOT_M1(close(STDIN_FILENO));
	// close the write end of the pipe
	CHECK_NOT_M1(close(fd[1]));
	// setup fd 1 to be correct
	CHECK_NOT_M1(dup2(fd[0], STDIN_FILENO));
	// the log fd...
	int logfd=-1;
	// buffer for the logging...
	const unsigned int bufsize=getpagesize();
	char* buf=new char[bufsize];
	unsigned int msg_count=0;
	while(true) {
		if(msg_count%1000==0) {
			if(logfd!=-1)
				CHECK_NOT_M1(close(logfd));
			logfd=getnextlogfile();
		}
		msg_count++;
		ssize_t size_read=CHECK_NOT_M1(read(STDIN_FILENO, buf, bufsize));
		if(size_read==0) {
			break;
		}
		// keep writing until everything is written...
		char* ptr=buf;
		while(size_read>0) {
			ssize_t size_write=CHECK_NOT_M1(write(logfd, ptr, size_read));
			size_read-=size_write;
			ptr+=size_write;
		}
	}
	delete[] buf;
}

int main(int argc, char** argv, char** envp) {
	int fd[2];
	CHECK_NOT_M1(pipe(fd));
	// child one
	int pid1=CHECK_NOT_M1(fork());
	if(pid1==0) {
		runUncontrolled(fd);
	}
	// child two
	int pid2=CHECK_NOT_M1(fork());
	if(pid2==0) {
		runLogger(fd);
	}
	// close the pipe at the parent
	// we cannot close before the fork or the children will not be able to use this
	// pipe...
	CHECK_NOT_M1(close(fd[0]));
	CHECK_NOT_M1(close(fd[1]));
	// wait for both children to die...
	int status;
	CHECK_NOT_M1(waitpid(pid1, &status, 0));
	CHECK_NOT_M1(waitpid(pid2, &status, 0));
	// here we can respawn the child if we want watchdog type functionality
	// too...
	return EXIT_SUCCESS;
}
