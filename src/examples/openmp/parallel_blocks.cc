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
#include <omp.h>// for omp_get_wtime(3), omp_get_thread_num(3), omp_get_num_procs(3), omp_get_max_threads(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This is an example of a parallel block of code. The for loop is not parallel
 * here, it is the block containting the for loop that is parallel.
 *
 * EXTRA_COMPILE_FLAGS=-fopenmp
 * EXTRA_LINK_FLAGS_AFTER=-fopenmp
 */

int main() {
	printf("C/OpenMP demo\n");
	printf("Number of processors available=%d\n", omp_get_num_procs());
	printf("Number of threads=%d\n", omp_get_max_threads());

	/*
	 * Measure the time before we start
	 */
	double wtime=omp_get_wtime();

	omp_set_num_threads(3);
	int id;
	#pragma omp parallel default(shared) private(id)
	{
		id=omp_get_thread_num();
		for(double i=1; i<10; i++) {
			printf(" i=%f, i*i=%f, %d\n", i, i*i, id);
		}
		;
		printf("This is process %d\n", id);
	}
	/*
	 * Finish up by measuring the elapsed time.
	 */
	wtime=omp_get_wtime()-wtime;
	printf("\n");
	printf("Normal end of execution.\n");
	printf("Elapsed wall clock time=%f\n", wtime);
	return EXIT_SUCCESS;
}
