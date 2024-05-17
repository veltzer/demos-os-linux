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
#include <stdio.h>	// for fprintf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, rand(3)

/*
 * This example shows how to treat constant vs non constant arguments
 * differently...
 *
 * TODO:
 * - stop the compile on constants or non constants...
 *
 * References:
 * http://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Other-Builtins.html
 */

inline void do_something_with_constant1(int a) __attribute__((always_inline));
inline void do_something_with_constant1(int a) {
	if(!__builtin_constant_p(a)) {
		printf("bad boy!\n");
	} else {
		printf("You are a good boy passing only constants... (%d)\n", a);
	}
}

#define ct_assert(e) (sizeof(char[1 - 2*!(e)]))
#define do_something_with_constant(a) \
	(__builtin_constant_p(a) ? 7:ct_assert(0==1))

int main() {
	// this will do something bad at runtime
	int x=rand();
	do_something_with_constant1(x);
	// this one will not
	do_something_with_constant1(13);
	return EXIT_SUCCESS;
}
