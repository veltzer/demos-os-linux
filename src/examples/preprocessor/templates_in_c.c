/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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
 * An example showing how to use the preprocessor in order to avoid
 * repetative code. In this case we do something like C++ templates
 * (type independent code) but in C.
 */

#define make_add(t) t add ## t(t a, t b) { return a+b; }

make_add(float);
make_add(double);

/*
 * float addfloat(float a,float b) {
 *	return a+b;
 * }
 * double adddouble(double a,double b) {
 *	return a+b;
 * }
 */

int main(int argc, char** argv, char** envp) {
	float x=4.5f;
	float y=3.2f;
	double dx=4.5f;
	double dy=3.2f;
	printf("%f+%f=%f\n", x, y, addfloat(x, y));
	printf("%lf+%lf=%lf\n", dx, dy, adddouble(dx, dy));
	return EXIT_SUCCESS;
}
