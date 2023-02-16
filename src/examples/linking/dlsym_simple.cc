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
#include <stdlib.h>	// for atoi(3), EXIT_SUCCESS
#include <dlfcn.h>	// for dlsym(3)
#include <math.h>	// for cos(3)

/*
 * This is a simple example of using dlsym
 *
 * EXTRA_LINK_FLAGS_AFTER=-ldl -lm
 */

int main(int argc, char** argv, char** envp) {
	double (*p_func)(double)=(typeof(p_func))dlsym(RTLD_NEXT, argv[1]);
	printf("p_func is %p\n", p_func);
	double val=atof(argv[2]);
	printf("this is just a junk print %lf\n", cos(val));
	printf("%s(%lf) is %lf\n", argv[1], val, p_func(val));
	return EXIT_SUCCESS;
}
