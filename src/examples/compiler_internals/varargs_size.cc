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
#include <stdio.h>	// for vprintf(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * The var args mechanism of C/C++ does not allow one to know exactly how many
 * arguments were passed.
 *
 * For example: the printf function assumes that the user has passed enough arguments
 * who'se type matches the % in the format string but it cannot ascertain it.
 *
 * Can we know how many var args were there quickly? Well, yes we could using a compile
 * time builtin compiler function called '__builtin_va_arg_pack_len'
 * Notes:
 * - the function returns the number of argument with disregard for their type.
 * This allows you to optimize a variadic API to use a non variadic version when that
 * version is not required.
 * - this builtin function can ONLY be used in a function that is GUARANTEED to be
 * inlined. This is not so with the regular 'inline' keyword. Instead the
 * '__attribute__((__gnu_inline__))' or '__attribute__((__always_inline__))'
 * must be used.
 */

// inline __attribute__((__gnu_inline__)) void trace(int first, ...) {
inline __attribute__((__gnu_inline__)) void trace(int first __attribute__((unused)), ...) {
	int i=__builtin_va_arg_pack_len();
	printf("__builtin_va_arg_pack_len() says %d\n", i);
}

int main() {
	int i=7;
	double d=3.14;
	trace(i, i, i);
	trace(1, 2, 3, 4);
	trace(d, d, d, d, d);
	trace(d, d, d, d, d, d, d, d, d);
	return EXIT_SUCCESS;
}
