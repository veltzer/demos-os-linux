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
#include <stdio.h>	// for printf(3), fprintf(3), stderr(O)
#include <stdlib.h>	// for EXIT_SUCCESS, malloc(3), free(3), EXIT_FAILURE, exit(3)
#include <strings.h>	// for bzero(3)
#include <err_utils.h>	// for CHECK_NOT_NULL()

/*
 * This is an improvement on the previous example.
 */

#define DEBUG

void _check_bounds(void* array, int pos) {
	int size=*((int*)array-1);
	if(pos<0 || pos>=size) {
		fprintf(stderr, "array access out of bounds. position [%d]. size [%d]\n", pos, size);
		exit(EXIT_FAILURE);
	}
}

#ifdef DEBUG
#define CHECK_BOUNDS(array, pos) _check_bounds(array, pos)
#else	// DEBUG
#define CHECK_BOUNDS(array, pos)
#endif	// DEBUG

static inline void* array_construct(int size) {
	int* p=(int*)CHECK_NOT_NULL(malloc((size+1)*sizeof(int)));
	bzero(p, (size+1)*sizeof(int));
	p[0]=size;
	return p+1;
}

inline void array_destruct(void* array) {
	int *p=(int*)array;
	free(p-1);
}

inline int array_size(void* array) {
	return *((int*)array-1);
}

inline int array_get_value(void* array, int pos) {
	CHECK_BOUNDS(array, pos);
	return ((int*)array)[pos];
}

inline void array_set_value(void* array, int pos, int val) {
	CHECK_BOUNDS(array, pos);
#pragma GCC diagnostic ignored "-Warray-bounds"
	((int*)array)[pos]=val;
}

int main() {
	const int size=10;
	int i;
	void* a=array_construct(size);
	for(i=0; i<size; i++) {
		printf("element [%d] is [%d]\n", i, array_get_value(a, i));
	}
	for(i=0; i<size; i++) {
		array_set_value(a, i, i);
	}
	for(i=0; i<size; i++) {
		printf("element [%d] is [%d]\n", i, array_get_value(a, i));
	}
	printf("array size is [%d]\n", array_size(a));
	// this next line will ext the application nicely in DEBUG mode
	// and will trample on memory in non DEBUG mode
	array_set_value(a, 13, 7);
	array_destruct(a);
	a=NULL;
	return EXIT_SUCCESS;
}
