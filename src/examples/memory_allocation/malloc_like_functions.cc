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
#include <stdlib.h>	// for malloc(3), calloc(3), realloc(3), free(3)
#include <stddef.h>	// for size_t

/*
 * This example shows how to correctly define your own malloc functions so that gcc will
 * work correctly with them.
 *
 * The use of the alloc_size attribute allows the compiled built in functions to know
 * the size of the object.
 *
 * The compiler uses these hints to know that the pointer returned by the functions is a new
 * pointer and therefore will not be equal to any pointer seen the in the past
 * (malloc and calloc should have it, realloc will not).
 *
 * Notes:
 * - show how the compiler uses the info in disassebly.
 */

void* my_calloc(size_t, size_t) __attribute__((alloc_size(1, 2), malloc));
void* my_realloc(void*, size_t) __attribute__((alloc_size(2)));
void* my_malloc(size_t) __attribute__((alloc_size(1), malloc));
void my_free(void*);	// no need for attributes of free

void* my_calloc(size_t nmemb, size_t size) {
	return calloc(nmemb, size);
}

void* my_realloc(void* ptr, size_t size) {
	return realloc(ptr, size);
}

void* my_malloc(size_t size) {
	return malloc(size);
}

void my_free(void* ptr) {
	free(ptr);
}

int main(int argc, char** argv, char** envp) {
	return EXIT_SUCCESS;
}
