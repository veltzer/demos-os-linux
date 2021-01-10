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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example explores the topic of initializing structures in C.
 *
 * If you do not do any initialisation then junk will appear (see a0).
 * If you state={} then all fields will be zeroed (see a1).
 * If you state={0} then all fields will be zeroed (see a2).
 * If you state={5} then the first field will be set to 5, all others to 0 (see a3).
 * If you state={6,7,8} then the values will initialize the fields in the order you required (see a4).
 * If you state={ .k=9 } (C99 style) then only that field will get the value, others will get 0 (see a5).
 * If you state={ j:10 } (GNU style) then only that field will get the value, others will get 0 (see a6).
 *
 * EXTRA_COMPILE_FLAGS=-Wno-error=uninitialized -w
 */

typedef struct _a {
	int i;
	int j;
	int k;
} a;

int main(int argc, char** argv, char** envp) {
	a a0;
	a a1={};
	a a2={0};
	a a3={5};
	a a4={6, 7, 8};
	a a5={ .k=9 };
	a a6={ j:10 };
	a a7;

	printf("a0.i=%d \n", a0.i);
	printf("a0.j=%d \n", a0.j);
	printf("a0.k=%d \n", a0.k);

	printf("a1.i=%d \n", a1.i);
	printf("a1.j=%d \n", a1.j);
	printf("a1.k=%d \n", a1.k);

	printf("a2.i=%d \n", a2.i);
	printf("a2.j=%d \n", a2.j);
	printf("a2.k=%d \n", a2.k);

	printf("a3.i=%d \n", a3.i);
	printf("a3.j=%d \n", a3.j);
	printf("a3.k=%d \n", a3.k);

	printf("a4.i=%d \n", a4.i);
	printf("a4.j=%d \n", a4.j);
	printf("a4.k=%d \n", a4.k);

	printf("a5.i=%d \n", a5.i);
	printf("a5.j=%d \n", a5.j);
	printf("a5.k=%d \n", a5.k);

	printf("a6.i=%d \n", a6.i);
	printf("a6.j=%d \n", a6.j);
	printf("a6.k=%d \n", a6.k);

	printf("a7.i=%d \n", a7.i);
	printf("a7.j=%d \n", a7.j);
	printf("a7.k=%d \n", a7.k);
	return EXIT_SUCCESS;
}
