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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <unistd.h>	// for close(2)
#include <sys/ioctl.h>	// for ioctl(2)
#include <errno.h>	// for errno(3)
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This is an example which explores how user space experiences errors coming from the kernel.
 *
 * Conclusions:
 * - If kernel returns a negative value (like most kernel code returns when there are errors)
 *	then user space will get -1 as a return value from the kernel function and errno will be
 *	set to the ** positive ** value of the error.
 * - If kernel returns a value which is greater or equal to 0 then user space will get the exact
 *	value that kernel returned and errno for the thread doing the call will not change.
 * - Please recall that errno is a PER THREAD variable.
 */

int main(int argc, char** argv, char** envp) {
	// file to be used
	const char *filename="/dev/drv_errno";
	// hold results of syscalls and errnos
	int res, myerrno;
	// default value for errno before we start
	const int def_errno=2005;

	printf("Starting, errno at start is %d\n", def_errno);
	int d=CHECK_NOT_M1(open(filename, O_RDWR));
	for(int i=-10; i < 10; i++) {
		errno=def_errno;
		res=ioctl(d, 0, i);
		myerrno=errno;
		printf("kernel returned %d, I got %d and errno is %d\n", i, res, myerrno);
	}
	CHECK_NOT_M1(close(d));
	return EXIT_SUCCESS;
}
