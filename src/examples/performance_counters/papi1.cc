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
#include <papi.h>	// for PAPI_* functions
#include <unistd.h>	// for usleep(3)
#include <stdio.h>	// for printf(3), fprintf(3)
#include <stdlib.h>	// for atoi(3), EXIT_FAILURE, EXIT_SUCCESS
#include <lowlevel_utils.h>	// for getticks()
#include <err_utils.h>	// for CHECK_INT(), CHECK_NOT_M1()

/*
 * Demo for using the PAPI library for RDTSC.
 * This example also proves that RDTSC is exactly what the PAPI library
 * is using under the hood.
 *
 * References:
 * http://icl.cs.utk.edu/papi/index.html
 *
 * EXTRA_LINK_FLAGS=-lpapi
 */

int main(int argc, char** argv, char** envp) {
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s [usecs]\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example is 1000000 which means 1 second\n", argv[0]);
		return EXIT_FAILURE;
	}
	unsigned int usecs=atoi(argv[1]);

	// init papi
	CHECK_INT(PAPI_library_init(PAPI_VER_CURRENT), PAPI_VER_CURRENT);

	// lets start measuring...
	long long c1=PAPI_get_real_cyc();
	ticks_t t1=getticks();
	CHECK_NOT_M1(usleep(usecs));
	long long c2=PAPI_get_real_cyc();
	ticks_t t2=getticks();
	printf("c1(papi) is %lld\n", c1);
	printf("c2(papi) is %lld\n", c2);
	printf("diff (papi): %lld\n", c2-c1);
	printf("t1(tsc) is %ld\n", t1);
	printf("t2(tsc) is %ld\n", t2);
	printf("diff (tsc): %ld\n", t2-t1);
	return EXIT_SUCCESS;
}
