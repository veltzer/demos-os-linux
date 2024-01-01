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

/*
 * check that exp(i*pi)==-1
 *
 * NOTES:
 * - the manual says that you have to link with the math library (-lm) in order to
 * use these functions but this does not seem to be the case.
 */

#include <firstinclude.h>
#include <math.h>	// for atan(3)
#include <complex.h>	// for cexp(3), creal(3), cimag(3)
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS

int main(int argc, char** argv, char** envp) {
	double pi=4.0*atan(1.0);
	// this should be "double complex" but we are in C++ and "complex" is in the
	// standard C++ library
	double _Complex z=cexp(I*pi);
	printf("%f+%f*i\n", creal(z), cimag(z));
	return EXIT_SUCCESS;
}
