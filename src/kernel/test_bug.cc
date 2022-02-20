/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <sys/ioctl.h>	// for ioctl(2)
#include <unistd.h>	// for close(2)
#include <err_utils.h>	// for CHECK_NOT_M1()
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This is a demo of how to use the BUG() macro in the kernel to find bugs.
 * Notice that this produces a stack trace in dmesg.
 * Notice also that you can still rmmod your module if you had a bug.
 */

int main(int argc, char** argv, char** envp) {
	// file to be used
	const char *filename="/dev/demo";
	// file descriptor to use
	int d=CHECK_NOT_M1(open(filename, O_RDWR));
	CHECK_NOT_M1(ioctl(d, 0, NULL));
	CHECK_NOT_M1(close(d));
	return EXIT_SUCCESS;
}
