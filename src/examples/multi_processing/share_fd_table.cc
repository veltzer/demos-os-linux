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
#include <unistd.h>	// for fork(2), getpid(2), getppid(2), sleep(3), lseek(2)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <stdio.h>	// for printf(3)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <sys/types.h>	// for getpid(2), getppid(2), open(2), lseek(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)

/*
 * This example demonstrates that parent and child processes in UNIX
 * share the file descriptor table after a call to fork(2).
 *
 * Note that the file descriptor numbers in the child and parent are the same
 * and the files that they point to are the same but the are *COPIES* of one
 * another (CLONE_FILES is NOT set in clone(2)). This means that the processes
 * do not effect each other (as good theory suggests).
 *
 * At fork(2) time child gets a copy of it's files file descriptors with their
 * state (mode, offset etc).
 *
 * The same applies for closing the file descriptors, changing attributes on
 * them (using fcntl(2) or the like), seeking the files and more.
 *
 * Note to device driver authors
 * If you a device driver author and the process using your device file
 * calls fork(2) then you will NOT GET ANY CALL IN YOUR DRIVER.
 * the same struct file* will be used by both parent and child.
 * this is similar to multi-threading.
 * So if your driver is multi-threaded ready it is also multi-process ready
 * and vice-versa.
 */

int main(int argc, char** argv, char** envp) {
	pid_t child_pid=CHECK_NOT_M1(fork());
	int fd=CHECK_NOT_M1(open("/etc/passwd", O_RDONLY));
	char buf[2];
	CHECK_NOT_M1(read(fd,&buf,2));
	const int count=10;
	if(child_pid) {
		// the parent
		for(int i=0;i<count;i++) {
			char c;
			CHECK_NOT_M1(read(fd,&c,1));
			off_t position = lseek(fd, 0, SEEK_CUR);
			printf("parent position %ld\n", position);
			sleep(1);
		}
	} else {
		// the child
		for(int i=0;i<count;i++) {
			off_t position = lseek(fd, 0, SEEK_CUR);
			printf("child position %ld\n", position);
			sleep(1);
		}
	}
	return EXIT_SUCCESS;
}
