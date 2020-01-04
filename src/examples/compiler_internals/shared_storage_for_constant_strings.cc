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
#include <assert.h>	// for assert(3)
#include <string.h>	// for strlen(3)

/*
 * This example shows that two constant strings share storage!
 * EXTRA_COMPILE_FLAGS=-fmerge-all-constants
 *
 * THIS EXAMPLE DOES NOT WORK
 *
 * References:
 * http://www.akkadia.org/drepper/ukuug2002slides.pdf
 */

const char* str2="what?!? this is a very long sentence";
const char* str1="this is a very long sentence";

int main(int argc, char** argv, char** envp) {
	printf("str1 is %p\n", str1);
	printf("str2 is %p\n", str2);
	assert(str1>=str2 && str1<str2+strlen(str2));
	return EXIT_SUCCESS;
}
