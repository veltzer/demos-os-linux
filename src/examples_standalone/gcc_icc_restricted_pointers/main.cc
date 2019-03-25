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
#include <stdlib.h>	// for EXIT_SUCCESS, random(3)
#include <measure.h>	// for measure, measure_init(), measure_start(), measure_end(), measure_print()

/*
 * Same example the example for gcc but now for the Intel compiler...
 */

#ifdef __GNUC__
#define myrestrict __restrict
#else
#define myrestrict restrict
#endif	// __GNUC__
// #define myrestrict

void add_check_restrict(int* arr, int num, int* result) __attribute__ ((noinline));
void add_check_restrict(int* arr, int num, int* result) {
	if(result<arr || result>arr+num) {
		int temp=0;
		for(int i=0; i<num; i++) {
			temp+=arr[i];
		}
		*result=temp;
	} else {
		*result=0;
		for(int i=0; i<num; i++) {
			*result+=arr[i];
		}
	}
}

void add_no_restrict(int* arr, int num, int* result) __attribute__ ((noinline));
void add_no_restrict(int* arr, int num, int* result) {
	*result=0;
	for(int i=0; i<num; i++) {
		*result+=arr[i];
	}
}

void add_restrict(int* myrestrict arr, int num, int* myrestrict result) __attribute__ ((noinline));
void add_restrict(int* myrestrict arr, int num, int* myrestrict result) {
	*result=0;
	for(int i=0; i<num; i++) {
		*result+=arr[i];
	}
}

void add_temp(int* arr, int num, int* result) __attribute__ ((noinline));
void add_temp(int* arr, int num, int* result) {
	int temp=0;
	for(int i=0; i<num; i++) {
		temp+=arr[i];
	}
	*result=temp;
}

int main(int argc, char** argv, char** envp) {
	// prepare a large array
	const unsigned int array_size=1000000;
	int* arr=new int[array_size];
	int res;
	for(unsigned int i=0; i<array_size; i++) {
		arr[i]=random();
	}
	const unsigned int loop=1000;

	measure m;
	measure_init(&m, "no restrict", loop);
	measure_start(&m);
	for(unsigned int i=0; i<loop; i++) {
		add_no_restrict(arr, array_size, &res);
	}
	measure_end(&m);
	measure_print(&m);

	measure_init(&m, "restrict", loop);
	measure_start(&m);
	for(unsigned int i=0; i<loop; i++) {
		add_restrict(arr, array_size, &res);
	}
	measure_end(&m);
	measure_print(&m);

	measure_init(&m, "temp", loop);
	measure_start(&m);
	for(unsigned int i=0; i<loop; i++) {
		add_temp(arr, array_size, &res);
	}
	measure_end(&m);
	measure_print(&m);
	return EXIT_SUCCESS;
}
