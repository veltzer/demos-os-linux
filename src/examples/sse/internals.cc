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

/*
 * This is a simple mmx example. The compiler, if passed the '-mmmx'
 * flag will notice that we are doing vector work and will use
 * an mmx instruction to parallelize the work.
 * see this using:
 * make src/examples/sse/simple.dis
 *
 * EXTRA_COMPILE_FLAGS=-mmmx -g -march=native
 */

#include <firstinclude.h>
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <x86intrin.h>	// for ??

/*
typedef union f4vector {
	v4sf v;
	float f[4];
} f4vector;
*/

int main(int argc, char** argv, char** envp) {
/*
	printf("sizeof(v4sf) is %d\n", sizeof(v4sf));
	printf("sizeof(f4vector) is %d\n", sizeof(f4vector));
	f4vector a, b, c;

	a.f[0] = 1; a.f[1] = 2; a.f[2] = 3; a.f[3] = 4;
	b.f[0] = 5; b.f[1] = 6; b.f[2] = 7; b.f[3] = 8;

	c.v = a.v + b.v;
	printf("%f, %f, %f, %f\n", c.f[0], c.f[1], c.f[2], c.f[3]);
*/
	return EXIT_SUCCESS;
}
