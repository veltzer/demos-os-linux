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
#include <unistd.h>	// for getpagesize(2), sysconf(3)
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This example shows how to get the systems page size, the available physical pages
 * and the overall physical pages (overall RAM size) of a system.
 * Note that you can see all this info from the command line using 'getconf -a | grep PAGE'
 */

int main(int argc, char** argv, char** envp) {
	printf("getpagesize() is %d\n", getpagesize());
	printf("sysconf(_SC_PAGESIZE) is %d\n", CHECK_NOT_M1(sysconf(_SC_PAGESIZE)));
	printf("sysconf(_SC_PAGE_SIZE) is %d\n", CHECK_NOT_M1(sysconf(_SC_PAGE_SIZE)));
	printf("sysconf(_SC_AVPHYS_PAGES) is %d\n", CHECK_NOT_M1(sysconf(_SC_AVPHYS_PAGES)));
	printf("sysconf(_SC_PHYS_PAGES) is %d\n", CHECK_NOT_M1(sysconf(_SC_PHYS_PAGES)));
	return EXIT_SUCCESS;
}
