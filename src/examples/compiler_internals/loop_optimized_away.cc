/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <stdio.h>	// for stderr, fprintf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <disassembly_utils.h>	// for disassemble_function()

/*
 * This example compares all sorts of loops that gcc can and
 * cannot optimize away.
 * Consider the question why ? Doesn't gcc simulate your code
 * in order to optimize it away ? The answer is that it does
 * not simulate your code but rather understand the underlying
 * construct to deduce what you are doing and use a predefined
 * set of formulas to precalculate the result...
 *
 * You can what code is indeed optimized away and what is
 * not when you disassemble the code.
 *
 * Currently, in gcc 4.9.1, the first two loops are optimized
 * away and the third isn't.
 *
 * EXTRA_COMPILE_FLAGS=-g3
 */

int main(int argc, char** argv, char** envp) {
	unsigned long sum1=0;
	for(int i=0; i<100; i++) {
		sum1+=i;
	}
	// we print so that the compile does not optimize out the loop
	fprintf(stderr, "sum1 is %lu\n", sum1);

	// for some strange reason gcc will only optimize the next
	// loop away if the sum is held in a variable of type 'int'...
	int sum2=0;
	for(int i=0; i<100; i++) {
		sum2+=i*i;
	}
	// we print so that the compile does not optimize out the loop
	fprintf(stderr, "sum2 is %d\n", sum2);

	unsigned long sum3=0;
	for(int i=0; i<100; i++) {
		sum3+=i*i*i;
	}
	// we print so that the compile does not optimize out the loop
	fprintf(stderr, "sum3 is %lu\n", sum3);

	disassemble_function("main");
	return EXIT_SUCCESS;
}
