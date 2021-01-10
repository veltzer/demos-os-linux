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
#include <sys/types.h>	// for open(2)
#include <sys/stat.h>	// for open(2)
#include <fcntl.h>	// for open(2)
#include <sys/ioctl.h>	// for ioctl(2)
#include <unistd.h>	// for close(2)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * A basic test which opens the device, runs one ioctl with 0 or user given
 * cmd and no data and then closes the device
 */
int main(int argc, char** argv, char** envp) {
	int ioctl_num=0;
	if (argc > 1) {
		ioctl_num=atoi(argv[1]);
	}
	// file to be used
	const char *filename="/dev/demo";
	// file descriptor
	int d=CHECK_NOT_M1(open(filename, O_RDWR));
	CHECK_NOT_M1(ioctl(d, ioctl_num, NULL));
	CHECK_NOT_M1(close(d));
	return EXIT_SUCCESS;
}
