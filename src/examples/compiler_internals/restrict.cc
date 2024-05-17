/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2024 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for EXIT_SUCCESS, random()
#include <measure.h>	// for measure, measure_init(), measure_start(), measure_end(), measure_print()

/*
 * This is an example of how to use the __restrict gcc feature
 * Note that the keyword "restrict" is not supported by gcc (I am on 4.6.1).
 * It may enter some C++ standard or it may not.
 *
 * What are the semantics of __restrict?
 * It tells the compiler that any access to the values pointed to by this pointer
 * will be done only through this pointer or derived pointers from this pointer.
 *
 * This means that if you write a function that gets two pointers and you add
 * restrict to them then you commit to them having no overlapped areas.
 *
 * Why should you use this? Because it allows the compiler to perform optimizations
 * that it could not do otherwise. In this example the compiler will not need
 * to store the value in *result to main memory and will be able to store
 * it in a register.
 *
 * If you want to see this in action create a disassembled version of this code
 * under different versions of the __restrict macro below...
 *
 * The difference in performance here is quite noticable...
 *
 * The next does not help to get it to the same performance as the intel compiler...
 * EXTRA_COMPILE_FLAGS=-mtune=corei7 -O3
 */

void add_no_restrict(int* arr, int num, int* result) __attribute__ ((noinline));
void add_no_restrict(int* arr, int num, int* result) {
	*result=0;
	for(int i=0; i<num; i++) {
		*result+=arr[i];
	}
}

void add_one_restrict(int* __restrict arr, int num, int* result) __attribute__ ((noinline));
void add_one_restrict(int* __restrict arr, int num, int* result) {
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

void add_restrict(int* __restrict arr, int num, int* __restrict result) __attribute__ ((noinline));
void add_restrict(int* __restrict arr, int num, int* __restrict result) {
	*result=0;
	for(int i=0; i<num; i++) {
		*result+=arr[i];
	}
}

int main() {
	// prepare a large array
	const unsigned int array_size=10000000;
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

	measure_init(&m, "one_restrict", loop);
	measure_start(&m);
	for(unsigned int i=0; i<loop; i++) {
		add_one_restrict(arr, array_size, &res);
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
