/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <math.h>	// for cos(3)
#include <stdio.h>	// for printf(3), fprintf(3), stderr:object
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, atof(3)

int main(int argc,char** argv,char** envp) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s x\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	double x=atof(argv[1]);
	printf("cos(%lf) is %lf\n", x, cos(x));
	//printf("sin(%lf) is %lf\n", x, sin(x));
	return EXIT_SUCCESS;
}
