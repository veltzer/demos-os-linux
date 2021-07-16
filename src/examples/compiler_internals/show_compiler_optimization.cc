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
#include <disassembly_utils.h>	// for disassemble_function()

/*
 * This example shows that the compiler is very aggresive on optimization.
 * It doesn't even unroll the loop! It simply calculates (1+99)*99/2=4950
 * and puts the value in the code (look for $0x1356 in the assembly).
 * Conclusion: DO NOT UNDERESTIMATE THE COMPILER!
 *
 * Notes:
 * - We cannot turn off optimization in the middle of a function (we get a compile
 * error for such an attempt) and that is why the function which is not optimized
 * is a standalone function.
 * - gcc 4.8 does not do this type of optimization even with -O3
 *
 * this is to make sure that source code interleaving in the disassembly works
 * out well...
 * EXTRA_COMPILE_FLAGS=-g3
 * this is to make sure that we compile using gcc 4.4 since newer gcc versions
 * do not do this type of optimization...
 * COMPILE=g++-4.8
 * LINKE=g++-4.8
 */

/*
 * This is a function which calculates the sum of numbers up to some number
 * but with #pragmas that make sure that the compiler does not optimize anything
 */
#pragma GCC push_options
#pragma GCC optimize ("O0")
unsigned int sum_not_optimized() __attribute__((noinline));
unsigned int sum_not_optimized() {
	unsigned int sum=0;
	for(unsigned int i=0; i<100; i++) {
		sum+=i;
	}
	return sum;
}
#pragma GCC pop_options

unsigned int sum_optimized() __attribute__((noinline));
unsigned int sum_optimized() {
	unsigned int sum=0;
	for(unsigned int i=0; i<100; i++) {
		sum+=i;
	}
	return sum;
}

unsigned int sum_print_opt() __attribute__((noinline));
unsigned int sum_print_opt() {
	unsigned int sum=0;
	for(unsigned int i=0; i<100; i++) {
		printf("i is %d\n", i);
		sum+=i;
	}
	return sum;
}

int main(int argc, char** argv, char** envp) {
	disassemble_function("sum_not_optimized");
	disassemble_function("sum_optimized");
	disassemble_function("sum_print_opt");
	return EXIT_SUCCESS;
}
