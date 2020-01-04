/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <omp.h>// for #pragma omg parallel, omp_get_thread_num(3), omp_get_num_threads(3)
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This is a minimal openmp program.
 * Notice that openmp automatically adjusts to the number of cores that you have.
 * You can ofcourse, change that (reduce or increase number of threads)
 *
 * EXTRA_COMPILE_FLAGS=-fopenmp
 * EXTRA_LINK_FLAGS=-fopenmp
 */

int main(int argc, char** argv, char** envp) {
	#pragma omp parallel
	printf("Hello from thread %d, nthreads %d\n", omp_get_thread_num(), omp_get_num_threads());
	return EXIT_SUCCESS;
}
