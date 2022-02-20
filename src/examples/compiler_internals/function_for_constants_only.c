/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for atoi(3)

/*
 * This example shows how to write a function that only works for constants.
 * This file is a C file because g++ does not have this feature yet.
 * A quick check on the internet shows that a patch to have this in g++ is on the way.
 * Notice that the trick is to create a compile time error and is based on returning
 * a void* instead of an int.
 */
static inline int _sum(int i) {
	return(i *(i + 1) / 2);
}

#define sum1(i)	\
	__builtin_choose_expr( \
	__builtin_constant_p(i), \
	_sum(i), \
	(void)0)

#define sum2(i)	\
	(__builtin_constant_p(i) ? _sum(i) : 0)

/*
 * This inline version does not work because of bug in gcc whereby __builtin_constant_p(i) is not a constant
 */

static inline int sum3(int i) {
	return __builtin_constant_p(i) ? _sum(i) : 0;
}

int main(int argc, char** argv, char** envp) {
	// everything is ok for constants
	printf("sum1 for 100 is %d\n", sum1(100));
	printf("sum2 for 5 is %d\n", sum2(5));
	printf("sum3 for 7 is %d\n", sum3(7));
	// we make sure that this is indeed a variable...:)
	int j=atoi("25");
	// only this creates a compile time error
	// printf("sum1 for j is %d\n",sum1(j));
	// this one just returns bad value
	printf("sum2 for j is %d\n", sum2(j));
	// this one just returns bad value
	printf("sum3 for j is %d\n", sum3(j));
	return(0);
}
