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
#include <sys/eventfd.h>// for eventfd(2)
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <unistd.h>	// for fork(2), close(2)
#include <sys/ioctl.h>	// for ioctl(2)
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This tests eventfd code in the kernel...
 *
 * TODO:
 * - call ioctl on the eventfd and get the signal...
 */
int main(int argc, char** argv, char** envp) {
	// file to be used
	const char *filename="/dev/mod_eventfd";

	int fd=CHECK_NOT_M1(open(filename, O_RDWR));
	CHECK_NOT_M1(eventfd(0, 0));
	int pid=CHECK_NOT_M1(fork());
	if(pid==0) {
		printf("Child\n");
	} else {
		printf("Parent\n");
	}
	CHECK_NOT_M1(close(fd));
	return EXIT_SUCCESS;
}
