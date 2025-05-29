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
#include <values.h>	// for MAXINT, MININT, MAXLONG, MINLONG, MAXFLOAT, MINFLOAT, MAXDOUBLE, MINDOUBLE
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example prints the limits of various types on the system using
 * the values.h file. limits.h may also be used for this.
 *
 * TODO:
 * - are there other values (MINUNSIGNEDINT?!?) in values.h that we can print here?
 */

int main() {
	printf("MAXINT is %d\n", MAXINT);
	printf("MININT is %d\n", MININT);
	printf("MAXLONG is %ld\n", MAXLONG);
	printf("MINLONG is %ld\n", MINLONG);
	printf("MAXFLOAT is %f\n", MAXFLOAT);
	printf("MINFLOAT is %f\n", MINFLOAT);
	printf("MAXDOUBLE is %lf\n", MAXDOUBLE);
	printf("MINDOUBLE is %lf\n", MINDOUBLE);
	return EXIT_SUCCESS;
}
