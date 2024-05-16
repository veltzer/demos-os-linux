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
#include <assert.h>	// for assert(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example shows how to add binary literals to the C++ language.
 * GCC actually has built in binary literals: int x=0b1010; but
 * this is pretty cool stuff as it is implemented using recursive template
 * programming.
 *
 * References:
 * http://code.google.com/p/cpp-binary-constants/source/browse/Binary.h
 * http://stackoverflow.com/questions/2611764/can-i-use-a-binary-literal-in-c-or-c
 * http://gcc.gnu.org/onlinedocs/gcc/Binary-constants.html
 */

/*
 * Largest integer constant available to the compiler
 *
 */
typedef unsigned long longest_t;

/*
 * The Class
 *
 */
template<longest_t N>
class bin {
public:
	enum {
		value = (N % 8) + (bin<N / 8>::value << 1)
	};
};

/*
 * Specialization for ending the chain
 *
 */
template<>
class bin<0> {
public:
	enum {
		value = 0
	};
};

/*
 * Macro-processing glue: force the number to be octal to both
 * end the recursion chain and make posible more digits
 *
 */
#define binary( n ) bin<0 ## n>::value

/*
 * Tests
 *
 */
int main() {
	assert(binary(11)==3);
	assert(binary(111)==7);
	assert(binary(1111)==15);
	assert(binary(11111111)==255);
	assert(binary(100000000)==256);
	assert(binary(0000000000001)==1);
	// GCC native only syntax
	assert(0b11==3);
	assert(0b111==7);
	assert(0b1111==15);
	assert(0b11111111==255);
	assert(0b100000000==256);
	assert(0b0000000000001==1);
	// this will fail
	// assert(binary(11)==4);
	return EXIT_SUCCESS;
}
