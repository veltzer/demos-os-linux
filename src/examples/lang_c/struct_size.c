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

/*
 * This example explores the sizeof structures.
 *
 * This example shows that:
 * - the size of an empty structure is 0 in C (in C++ it's actually 1 to make it an object).
 * - just one character does NOT pad the size to 4 or something.
 */

typedef struct _empty {
} empty;

typedef struct _onechar {
	char mychar;
} onechar;

int main(int argc, char** argv, char** envp) {
	printf("sizeof(empty) is %zd\n", sizeof(empty));
	printf("sizeof(onechar) is %zd\n", sizeof(onechar));
	return EXIT_SUCCESS;
}
