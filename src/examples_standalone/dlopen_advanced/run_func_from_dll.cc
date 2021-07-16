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
#include <stdio.h>	// for printf(3), fprintf(3)
#include <dlfcn.h>	// for dlopen(3), dlsym(3), dlclose(3)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, atof(3)
#include <err_utils.h>	// for CHECK_NOT_NULL(), CHECK_ZERO()

int main(int argc, char** argv, char** envp) {
	if(argc!=4) {
		fprintf(stderr, "%s: usage: %s [library] [function] [value]\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	const char* p_lib=argv[1];
	const char* p_func=argv[2];
	const char* p_sval=argv[3];
	const double p_dval=atof(p_sval);
	void* h=CHECK_NOT_NULL(dlopen(p_lib, RTLD_NOW));
	void* sym=CHECK_NOT_NULL(dlsym(h, p_func));
	double (*f)(double)=(double (*)(double))sym;
	// another option
	// double (*f)(double)=(typeof(f))sym;
	double result=f(p_dval);
	printf("result is %lf\n", result);
	CHECK_ZERO(dlclose(h));
	return EXIT_SUCCESS;
}
