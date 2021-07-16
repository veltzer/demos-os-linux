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
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, atof(3)
#include <stdio.h>	// for printf(3)

/*
 * This is an example showing how to use the soft floating
 * point facilities of the gnu c library.
 *
 * to see the content of the soft float library:
 * nm /usr/lib/gcc/i686-linux-gnu/4.8/x32/libgcc.a | grep div
 *
 * this is so disassembly will show interleaved code
 * EXTRA_COMPILE_FLAGS=-g3
 * EXTRA_LINK_FLAGS=-lgcc
 */

extern "C" double __bid64dq_div(double, double);
extern "C" float __divtf3(float, float);

int main(int argc, char** argv, char** envp) {
	if(argc!=3) {
		fprintf(stderr, "%s: usage: %s [a] [b]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	float a=atof(argv[1]);
	float b=atof(argv[2]);
	// printf("%lf/%lf is %lf\n", a, b, a/b);
	printf("__divid(%f,%f) is %f\n", a, b, __divtf3(0.5f, 0.4f));
	return EXIT_SUCCESS;
}
