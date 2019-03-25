/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for printf(3)
#include <stdint.h>	// for INT32_MAX, UINT64_MAX, INT64_MAX
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This shows how to define various long related constants and how to print them.
 */

int main(int argc, char** argv, char** envp) {
	// this next line will not work (the LL is required...)
	// unsigned long long l1=214748364899;
	unsigned long long l1=214748364899LL;
	fprintf(stderr, "l1 is %llu\n", l1);
	long l2=INT32_MAX;
	fprintf(stderr, "l2 is %ld\n", l2);
	unsigned long long l3=UINT64_MAX;
	fprintf(stderr, "l3 is %llu\n", l3);
	long long l4=INT64_MAX;
	fprintf(stderr, "l4 is %lld\n", l4);
	// long long max=9223372036854775807LL;
	fprintf(stderr, "sizeof(3.14) is %zd\n", sizeof(3.14));
	fprintf(stderr, "sizeof(3.14f) is %zd\n", sizeof(3.14f));
	// float f=1.6676;
	// double d=1.8988f;
	return EXIT_SUCCESS;
}
