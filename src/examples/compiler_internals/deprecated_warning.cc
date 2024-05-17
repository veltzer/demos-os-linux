/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2024 Mark Veltzer <mark.veltzer@gmail.com>
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
 * This is a demo of how to write a function for which the user should not use
 * (deprecated).
 *
 * There are many more attributes that you can attach to functions in order to
 * get more checks from the compiler at compile time and get better code. Check
 * 'info gcc' for more details.
 */

// this is how to use unused in a function declaration...
// this is the right place to put the __attribute__...
int add(int a, int b) __attribute__((deprecated));

// now for the function definition (you cant put the __attribute__ here)...
int add(int a, int b) {
	return a + b;
}

int main() {
	const int a=5;
	const int b=5;

	// the next line two uses produce a compile time error
	// if gcc warnings hadn't been turned off...
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	add(5, 6);
	printf("%d+%d is %d\n", a, b, add(a, b));
	#pragma GCC diagnostic pop
	return EXIT_SUCCESS;
}
