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
// #define NDEBUG
#include <assert.h>	// for assert(3), assert_perror(3)
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <sys/ioctl.h>	// for ioctl(2)

/*
 * Demo the use of glibc assert in C/C++
 *
 * - assert_perror macro also exists and allows to validate correct results
 * from system calls and print the right errors in case of error.
 * - the macro NDEBUG controlls whether asserts reduce to nothing or to actual
 * checks at runtime.
 * - some people control NDEBUG not from the source code but rather from the
 * build system where they either light it up for the entire project, turn
 * it off for the entire project, or select a subset of translation units
 * to turn it on for.
 * - people who do not set NDEBUG at the project build level but rather at
 * the individual module level usually include it the regular way taking
 * heed to set NDEBUG before the include (#define for no assertions, #undef
 * or nothing for assertions).
 * - you can enable asserts for sections in your code by including assert.h
 * whenever you change the value of the NDEBUG macro. This is not something
 * most people are comfortable doing.
 * - when an assertion fails it prints the executable name, source file name,
 * line number, function (unmangled) and the assertion code itself that failed.
 * It them proceeds to call abort (you can see all of this using strace).
 * - assert(X) should never produce side effects so that the program will behave
 * the same whether assertions are enabled or not.
 * - The `assert' facility is designed for detecting _internal inconsistency_;
 * it is not suitable for reporting invalid input or improper usage to the
 * _user_ of the program.
 */

int main(int argc, char** argv, char** envp) {
	assert(1==1);
	printf("in here 1\n");
#define NDEBUG
#include <assert.h>	// for assert(3)
	assert(1==2);
	printf("in here 2\n");
#undef NDEBUG
#include <assert.h>	// for assert(3)
	// assert(3==4);
	assert_perror(ioctl(7, 8));
	printf("in here 3\n");
	return EXIT_SUCCESS;
}
