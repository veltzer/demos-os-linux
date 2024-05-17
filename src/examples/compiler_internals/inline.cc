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
 * This example explores the different ways to inline a function.
 * There are three ways to do this:
 * - 'inline'.
 * - '__inline__'.
 * - '__attribute__((gnu_inline))'.
 * - '__attribute__((__gnu_inline__))'.
 * - '__attribute__((always_inline))'.
 * - '__attribute__((__always_inline__))'.
 *
 * NOTES:
 * - the three different methods do not mean the same thing.
 * - regular inline means inline for speed if speed is of the essense which
 * means that when compiling without optimization inline is not guaranteed.
 * - always inline means always inline (even when compiling without optimization).
 * and does not work in new gcc's which give warnings/errors about no guarantee for inlining.
 * - gnu_inline depends on other flags and has an intermediate meaning.
 */

static inline int add1(int a, int b) {
	return a+b;
}
static __inline__ int add2(int a, int b) {
	return a+b;
}
static inline __attribute__((gnu_inline)) int add3(int a, int b) {
	return a+b;
}
static inline __attribute__((__gnu_inline__)) int add4(int a, int b) {
	return a+b;
}
/*
 * static __attribute__((always_inline)) int add5(int a, int b) {
 * return a+b;
 * }
 * static __attribute__((__always_inline__)) int add6(int a, int b) {
 * return a+b;
 * }
 */

int main() {
	int a=2;
	int b=2;
	printf("add1(%d,%d)=%d\n", a, b, add1(a, b));
	printf("add2(%d,%d)=%d\n", a, b, add2(a, b));
	printf("add3(%d,%d)=%d\n", a, b, add3(a, b));
	printf("add4(%d,%d)=%d\n", a, b, add4(a, b));
// printf("add5(%d,%d)=%d\n",a,b,add5(a,b));
// printf("add5(%d,%d)=%d\n",a,b,add6(a,b));
	return EXIT_SUCCESS;
}
