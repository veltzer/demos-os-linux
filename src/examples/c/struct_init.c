/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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
 * This is an example of initializing a struct in various ways in C...
 */

struct mystruct{
	int a;
	int b;
	int c;
};

int main() {
	// this is C99 style. It doesn't require any specific flags to gcc these days
	// fields not initialized will be 0
	struct mystruct v1={ .a=6, .b=7 };
	printf("v1.a is %d\n", v1.a);
	printf("v1.b is %d\n", v1.b);
	printf("v1.c is %d\n", v1.c);
	// this is the GNU style...
	// fields not initialized will be 0
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
	struct mystruct v2={ a: 8, b: 9 };
#pragma GCC diagnostic pop
	printf("v2.a is %d\n", v2.a);
	printf("v2.b is %d\n", v2.b);
	printf("v2.c is %d\n", v2.c);
	// this will always work (values will be junk)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
	struct mystruct v3;
	// cppcheck-suppress uninitStructMember
	// cppcheck-suppress uninitvar
	printf("v3.a is %d\n", v3.a);
	// cppcheck-suppress uninitStructMember
	printf("v3.b is %d\n", v3.b);
	// cppcheck-suppress uninitStructMember
	printf("v3.c is %d\n", v3.c);
	// this will also always work but because we are accessing the values later
	// then gcc will 0 them for us...
	struct mystruct v4;
	// cppcheck-suppress uninitStructMember
	// cppcheck-suppress uninitvar
	printf("v4.a is %d\n", v4.a);
	// cppcheck-suppress uninitStructMember
	printf("v4.b is %d\n", v4.b);
	// cppcheck-suppress uninitStructMember
	printf("v4.c is %d\n", v4.c);
#pragma GCC diagnostic pop
	v4.a=10;
	v4.b=11;
	v4.c=12;
	printf("v4.a is %d\n", v4.a);
	printf("v4.b is %d\n", v4.b);
	printf("v4.c is %d\n", v4.c);
	return EXIT_SUCCESS;
}
