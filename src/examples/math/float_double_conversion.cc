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
#include <stdio.h>	// for printf(3), fprintf(3), stderr:object
#include <stdlib.h>	// for atof(3), EXIT_FAILURE, EXIT_SUCCESS

/*
 * This is an exploration of how the compiler coverts between float and
 * double.
 *
 * Note:
 * Some of these lines will be caught if you add -Wconversion flag to gcc.
 */

/*
 * A regular float function...
 */
float my_float_func(float a, float b) {
	// the next line will keep the compiler from optimizing away my function
	// (damn smartass compiler...)
	asm ("");
	return a*b;
}
float my_float_func(float a, float b) __attribute__((noinline));
/*
 * A double precision function...
 */
double my_double_func(double a, double b) {
	// the next line will keep the compiler from optimizing away my function
	// (damn smartass compiler...)
	asm ("");
	return a*b;
}
double my_double_func(double a, double b) __attribute__((noinline));

int main(int argc, char** argv, char** envp) {
	if(argc!=3) {
		fprintf(stderr, "%s: usage %s [float] [double]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	// a float and double which will NEVER be optimized away by the compiler
	// since their origin is the user...
	float f=(float)atof(argv[1]);
	double d=atof(argv[2]);
	printf("%f\n", my_float_func(f, f));
	printf("%lf\n", my_double_func(f, f));
	printf("%f\n", my_float_func(d, d));
	printf("%lf\n", my_double_func(d, d));
	printf("%f\n", my_float_func(5.5f, 6.7f));
	printf("%lf\n", my_double_func(5.5f, 6.7f));
	printf("%f\n", my_float_func(5.5, 6.7));
	printf("%lf\n", my_double_func(5.5, 6.7));
	return EXIT_SUCCESS;
}
