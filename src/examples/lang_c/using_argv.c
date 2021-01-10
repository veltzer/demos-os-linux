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
#include <stdio.h>	// for printf(3)
#include <string.h>	// for strcmp(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example shows that you can create a single binary that will behave
 * differently according to the name by which it is called. This trick is used
 * heavily in Linux. Examples are the fgrep, grep and egrep triplet, the busybox
 * implementation and more.
 */

int main(int argc, char** argv, char** envp) {
	if (strcmp(argv[0], "./add")==0) {
		printf("Im adding");
	}
	if (strcmp(argv[0], "./sub")==0) {
		printf("Im subtracting");
	}
	return EXIT_SUCCESS;
}
