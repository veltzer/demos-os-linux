/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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

#include <firstinclude.h>
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example shows that C compilers do shortcutting by default in boolean expressions.
 */

int global;

/*
 * This functions always return true but the compiler is too stupid to figure it out...
 */
int always_false() {
	global+=1;
	return 0;
}

int main(int argc, char** argv, char** envp) {
	global=0;
	if(always_false() && always_false()) {
		printf("You should not be here...\n");
	}
	printf("global is %d\n", global);
	return EXIT_SUCCESS;
}
