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
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This is the most basic hello, world program for C
 */

// the arguments to main which are not used DO NOT
// create a compilation error even with all warnings turned
// on
// also main DOES NOT have to return int
int main(int argc, char** argv, char** envp) {
	printf("Hello, World!\n");
	// you really don't have to return a value from main
	// and if you forget this the compiler will not scream.
	// It is a good idea in any case.
	return EXIT_SUCCESS;
}
