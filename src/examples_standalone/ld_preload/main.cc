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

#include <firstinclude.h>
#include <stdio.h>	// for fprintf(3), printf(3)
#include <math.h>	// for sin(3)
#include <stdlib.h>	// for atof(3), EXIT_SUCCESS, EXIT_FAILURE

/*
 * The function runs the sin function on data from the outside world because
 * otherwise the wiseass compiler would computer the sin function at compiler
 * time!
 */

int main(int argc, char** argv, char** envp) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [x]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	double val=atof(argv[1]);
	printf("sin(%lf) is %lf\n", val, sin(val));
	return EXIT_SUCCESS;
}
