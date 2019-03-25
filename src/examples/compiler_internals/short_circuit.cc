/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for atoi(3)

/*
 * Lets see if short circuit bool expression evaluation works...
 * You determine (via args) whether the first expression or second
 * expression is 1 or 0.
 * The reason that I'm using argv here is in order for the compilers
 * optimizer not to know ahead of time what the result would be...
 */
int do_something(int val) {
	printf("Im doing something...\n");
	return(val);
}

int main(int argc, char** argv, char** envp) {
	if (argc!=3) {
		printf("usage: %s [val1] [val2]\n", argv[0]);
		return(-1);
	}
	if (atoi(argv[1]) || do_something(atoi(argv[2]))) {
		printf("Inside the if statement\n");
	} else {
		printf("Outside the if statement\n");
	}
	return EXIT_SUCCESS;
}
