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
#include <stddef.h>	// for offsetof(3)

/*
 * This is an example of finding out how far a field is from the begining
 * of the structure it is embedded in.
 * In a similar example in C++ (using g++ instead of gcc) we had to
 * use '1' instead of '0' in these next macros because g++ is smarter
 * than gcc and translated the 0 to NULL and disallowed it use.
 * In this example, since we are compiling to C we can use 0 freely.
 * Notice that with my macro, offsetof and __builtin_offsetof you can use the
 * "struct" prefix to the structure name or drop it.
 */

#define myoffsetof(structname, fieldname) ((char *)(&(((structname *)0)->fieldname)) - (char *)0)

// our own struct for the demo...
typedef struct _mystruct {
	int field_a;
	int field_b;
	int field_c;
} mystruct;

int main(int argc, char** argv, char** envp) {
	// using our own macro
	printf("field_a is in offset %zd\n", myoffsetof(mystruct, field_a));
	printf("field_b is in offset %zd\n", myoffsetof(struct _mystruct, field_b));
	printf("field_c is in offset %zd\n", myoffsetof(mystruct, field_c));
	// using a libc macro
	printf("field_a is in offset %zd\n", offsetof(mystruct, field_a));
	printf("field_b is in offset %zd\n", offsetof(struct _mystruct, field_b));
	printf("field_c is in offset %zd\n", offsetof(mystruct, field_c));
	// using a gcc builtin
	printf("field_a is in offset %zd\n", __builtin_offsetof(mystruct, field_a));
	printf("field_b is in offset %zd\n", __builtin_offsetof(struct _mystruct, field_b));
	printf("field_c is in offset %zd\n", __builtin_offsetof(mystruct, field_c));
	return(0);
}
