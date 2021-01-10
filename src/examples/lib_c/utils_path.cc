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
#include <stdlib.h>	// for EXIT_SUCCESS, malloc(3), free(3)
#include <stdio.h>	// for printf(3)
#include <err_utils.h>	// for CHECK_POSITIVE(), CHECK_NOT_NULL(), CHECK_IN_RANGE()
#include <unistd.h>	// for confstr(3)

/*
 * This example extracts the path to POSIX.2 utils
 * This example is taken from 'man 3 confstr'
 */

int main(int argc, char** argv, char** envp) {
	size_t n=CHECK_POSITIVE(confstr(_CS_PATH, NULL, (size_t)0));
	char *pathbuf=(char*)CHECK_NOT_NULL(malloc(n));
	CHECK_IN_RANGE(confstr(_CS_PATH, pathbuf, n), 1, n+1);
	printf("confstr(_CS_PATH) returned %s\n", pathbuf);
	free(pathbuf);
	return EXIT_SUCCESS;
}
