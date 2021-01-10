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
#include <stdlib.h>	// for EXIT_SUCCESS, malloc(3), free(3)
#include <strings.h>	// for bzero(3)
#include <err_utils.h>	// for CHECK_NOT_NULL()

/*
 * This is an example of encapsulating various sized arrays into
 * an object and doing so efficiently without a need to dereference.
 * In this case this is an array of integers.
 */

static inline void* array_construct(int size) {
	int* p=(int*)CHECK_NOT_NULL(malloc(size*sizeof(int)));
	bzero(p, size*sizeof(int));
	return p;
}

inline void array_destruct(void* array) {
	free(array);
}

inline int array_get_value(void* array, int pos) {
	return ((int*)array)[pos];
}

inline void array_set_value(void* array, int pos, int val) {
	((int*)array)[pos]=val;
}

int main(int argc, char** argv, char** envp) {
	const int size=10;
	int i;
	void* a=array_construct(size);
	for(i=0;i<size;i++) {
		printf("element [%d] is [%d]\n", i, array_get_value(a, i));
	}
	for(i=0;i<size;i++) {
		array_set_value(a, i, i);
	}
	for(i=0;i<size;i++) {
		printf("element [%d] is [%d]\n", i, array_get_value(a, i));
	}
	array_destruct(a);
	a=NULL;
	return EXIT_SUCCESS;
}
