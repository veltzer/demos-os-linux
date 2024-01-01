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
#include <stdio.h>	// for fprintf(3), stderr
#include <stdlib.h>	// for EXIT_SUCCESS, exit(3)

/*
 * This is a demo of how to write a function which declares that it
 * never returns.
 *
 * When do you write a function like that?
 * - when your function has an endless loop in it.
 * - when your function is an error handler and calls exit(3) or _exit(2)
 * - other situations?
 *
 * If you do not delcare your function as never returning the compiler
 * will force functions using your function to return values even after
 * calling your function. So you need to do the right thing for your
 * users...
 */

// attributes of functions *must* be declared on function declartion
// and not definition.
// The next function never returns (look and the code and see why).
void error(const char* msg, int exitcode) __attribute__((noreturn));

// now for the function definition (you cant put the __attribute__ here)...
void error(const char* msg, int exitcode) {
	fprintf(stderr, "error: %s\n", msg);
	exit(exitcode);
}

int minus1(int x) {
	if(x>0) {
		return x-1;
	}
	error("x must be positive", 2);
	// this next line is not neccessary since the compiler knows the the line above
	// does not return!
	// if yoo do not declare the above error function as 'noreturn' then the next
	// line will be neccessary to avoid compilation errors.
	//return -1;
}

int main(int argc, char** argv, char** envp) {
	printf("minus1 of 7 is %d\n", minus1(7));
	return EXIT_SUCCESS;
}
