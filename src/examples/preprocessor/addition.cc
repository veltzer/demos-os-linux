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
 * This shows how to do additions using the C pre-processor.
 *
 * References:
 * http://stackoverflow.com/questions/3539549/can-i-add-numbers-with-the-c-c-preprocessor
 *
 * TODO:
 * - enhance this example to handle numbers greater than 10.
 */

/* Now we know how to do addition to up to 1. */
#define INC_0 1
#define INC_1 2
#define INC_2 3
#define INC_3 4
#define INC_4 5
#define INC_5 6
#define INC_6 7
#define INC_7 8
#define INC_8 9
#define INC_9 10
#define INC(x) INC_ ## x

/* To add to even larger numbers, you need some sort of "recursion". */
#define ADD_0(x) x
#define ADD_1(x) INC(x)
#define ADD_2(x) ADD_1(INC(x))
#define ADD_3(x) ADD_2(INC(x))
#define ADD_4(x) ADD_3(INC(x))
#define ADD_5(x) ADD_4(INC(x))
#define ADD_6(x) ADD_5(INC(x))
#define ADD_7(x) ADD_6(INC(x))
#define ADD_8(x) ADD_7(INC(x))
#define ADD_9(x) ADD_8(INC(x))
#define ADD_10(x) ADD_9(INC(x))
#define ADD(x, y) ADD_ ## x(y)

int main() {
	printf("did you know that 2+3=%d\n", ADD(2, 3));
	return EXIT_SUCCESS;
}
