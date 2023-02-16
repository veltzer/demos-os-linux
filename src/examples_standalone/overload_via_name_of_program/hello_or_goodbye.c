/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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

#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <string.h>	// for strcmp(3)

/*
 * This is the most basic hello, world program for C
 */

// the arguments to main which are not used DO NOT
// create a compilation error even with all warnings turned
// on
// also main DOES NOT have to return int
int main(int argc, char** argv, char** envp) {
	if(strcmp(argv[0],"./hello.out")==0) {
		printf("Hello, World!\n");
		return EXIT_SUCCESS;
	}
	if(strcmp(argv[0],"./goodbye.out")==0) {
		printf("Goodbye, World!\n");
		return EXIT_SUCCESS;
	}
	printf("What do you want from me?!?\n");
	return EXIT_SUCCESS;
}
