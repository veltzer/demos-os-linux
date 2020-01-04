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
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>	// for EXIT_SUCCESS
#include <unistd.h>
#include <sys/ioctl.h>
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This tests the get_free_pages driver
 */

int main(int argc, char** argv, char** envp) {
	// file to be used
	const char *filename="/dev/demo";
	int d=CHECK_NOT_M1(open(filename, O_RDWR));
	CHECK_NOT_M1(close(d));
	return EXIT_SUCCESS;
}
