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
#include <us_helper.h>	// for mylikely(), myunlikely()

/*
 * This is an example of giving hints to the compiler about branchings.
 * Note that:
 * - some cpus have branch prediction in hardware and ignore such hints.
 * - some cpus have branch prediction in hardware and accept such hints.
 * - you can pass flags to the compiler to ignore these hints and create
 * regular branch instructions.
 * - some compiler and/or compiler versions do not actually use this information.
 *
 * Where can this help?
 * It can allow the compiler to reorder the control-flow graph to reduce the number of branches taken for the 'likely' path. This can have a marked improvement in loops where you're checking multiple exit cases.
 *
 * TODO:
 * - on the command line using time(1) this example runs the same with branch prediction and without.
 *	Modify this to have a marked effect on performance and measure itself so that I can show when
 *	this is effective.
 */

int main(int argc, char** argv, char** envp) {
	int x=0;
	long long sum=0;
	while(mylikely(x<1000000000)) {
		// mispredict on purpose...
		// while(myunlikely(x<1000000000)) {
		// to force the compiler to actually DO the loop.
		// moving this line to sum+=x will actually mean that the compiler
		// will calculate the whole loop in advance!!!
		sum+=x*x;
		x++;
	}
	printf("sum is %lld\n", sum);
	printf("x is %d\n", x);
	return EXIT_SUCCESS;
}
