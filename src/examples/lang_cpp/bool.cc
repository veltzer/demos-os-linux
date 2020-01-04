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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <us_helper.h>	// for stringify()

/*
 * This is an example explosing the 'bool' built in type in C++.
 *
 * Notes:
 * - bool, true, false, _Bool are all built in in C++.
 * - bool is small (1 byte)
 * - the C type for bool from 'stdbool.h': _Bool is supported by C++.
 * - casts to bool from int is possible (!=0 -> true, ==0 -> false).
 * - casts from bool to int are also possible (true -> 1, false -> 0).
 * - casts to bool from pointer is possible (!=0 -> true, ==0 -> false).
 * - casts from bool to pointer are also possible (true -> 0x1, false -> NULL).
 */

int main(int argc, char** argv, char** envp) {
	printf("sizeof(_Bool) is %zd\n", sizeof(_Bool));
	printf("sizeof(bool) is %zd\n", sizeof(bool));
	printf("_Bool is really %s\n", stringify(_Bool));
	printf("bool is really %s\n", stringify(bool));
	printf("true is really %s\n", stringify(true));
	printf("false is really %s\n", stringify(false));
	int itrue=(int)true;
	int ifalse=(int)false;
	printf("true cast to int is %d\n", itrue);
	printf("false cast to int is %d\n", ifalse);
	int i7=7;
	int i0=0;
	bool ib0=(bool)i0;
	bool ib7=(bool)i7;
	if(ib0) {
		printf("bool(0) is true\n");
	} else {
		printf("bool(0) is false\n");
	}
	if(ib7) {
		printf("bool(7) is true\n");
	} else {
		printf("bool(7) is false\n");
	}
	char* p0=(char*)0;
	char* p7=(char*)7;
	bool pb0=(bool)p0;
	bool pb7=(bool)p7;
	if(pb0) {
		printf("bool((char*)0) is true\n");
	} else {
		printf("bool((char*)0) is false\n");
	}
	if(pb7) {
		printf("bool((char*)7) is true\n");
	} else {
		printf("bool((char*)7) is false\n");
	}
	char* ptrue=(char*)true;
	char* pfalse=(char*)false;
	printf("true cast to char* is %p\n", ptrue);
	printf("false cast to char* is %p\n", pfalse);
	return EXIT_SUCCESS;
}
