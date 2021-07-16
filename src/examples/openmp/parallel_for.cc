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
#include <cmath>// for std::sin
#include <stdio.h>	// for printf(3)
#include <omp.h>// for #pragma omp parallel for
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * A demo calculating the sin table with openmp.
 *
 * EXTRA_COMPILE_FLAGS=-fopenmp
 * EXTRA_LINK_FLAGS=-fopenmp
 */

int main(int argc, char** argv, char** envp) {
	const int size=256;
	double sinTable[size];
	#pragma omp parallel for
	for(int n=0; n<size; ++n) {
		printf("thread %d calculating %d\n", omp_get_thread_num(), n);
		sinTable[n]=std::sin(2 * M_PI * n / size);
	}
	// the table is now initialized
	for(int i=0; i<size; i++) {
		printf("sinTable[%d]=%lf\n", i, sinTable[i]);
	}
	return EXIT_SUCCESS;
}
