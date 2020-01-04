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
#include <malloc.h>	// for malloc(3) and the hooks
// #define NDEBUG
#include <assert.h>	// for assert(3)
#include <iostream>	// for std::exception
#include <stdlib.h>	// for exit(3), EXIT_FAILURE, EXIT_SUCCESS
#include <trace_utils.h>// for TRACE()

/*
 * This program shows how you can disable malloc completely so that you will be sure
 * that in the running phase of your real-time application you are not using the malloc
 * functions which are sometimes not O(1) bound...
 *
 * This example uses malloc hooks in the C library to disable malloc.
 * Another way to achieve this is for you write your own malloc allocator to completely
 * replace malloc, or call malloc. Your allocator would provice the standard malloc
 * functions (malloc,realloc,memalign) and would throw exceptions or returns nulls
 * if these are called after a certain stage.
 *
 * EXTRA_COMPILE_FLAGS=-Wno-error=deprecated-declarations -Wno-deprecated-declarations
 */

static bool malloc_allowed=true;

// Variables to save original hooks
void* (*old_malloc_hook)(size_t, const void *);
void* (*old_realloc_hook)(void*, size_t, const void *);
void* (*old_memalign_hook)(size_t, size_t, const void *);
void (*old_free_hook)(void*, const void *);

// prototypes for my hooks
void* my_malloc_hook(size_t size, const void *caller);
void* my_realloc_hook(void *ptr, size_t size, const void *caller);
void* my_memalign_hook(size_t alignment, size_t size, const void *caller);
void my_free_hook(void *ptr, const void *caller);

static void save_hooks(void) {
	old_malloc_hook=__malloc_hook;
	old_realloc_hook=__realloc_hook;
	old_memalign_hook=__memalign_hook;
	old_free_hook=__free_hook;
}

static void setup_hooks(void) {
	__malloc_hook=my_malloc_hook;
	__realloc_hook=my_realloc_hook;
	__memalign_hook=my_memalign_hook;
	__free_hook=my_free_hook;
}

static void restore_hooks(void) {
	__malloc_hook=old_malloc_hook;
	__realloc_hook=old_realloc_hook;
	__memalign_hook=old_memalign_hook;
	__free_hook=old_free_hook;
}

void* my_malloc_hook(size_t size, const void *caller) {
	if(malloc_allowed) {
		restore_hooks();
		void* result=malloc(size);
		save_hooks();
		setup_hooks();
		return result;
	} else {
		// c alternative (termination)
		// restore_hooks();
		// TRACE("ERROR! Malloc is not allowed");
		// exit(EXIT_FAILURE);

		// cpp alternative (exception throwing)
		// restore_hooks();
		// TRACE("ERROR! Malloc is not allowed");
		// throw new std::exception();

		// c alternative (returning NULL)
		restore_hooks();
		return NULL;
	}
}

void* my_realloc_hook(void *ptr, size_t size, const void *caller) {
	throw std::exception();
}

void* my_memalign_hook(size_t alignment, size_t size, const void *caller) {
	throw std::exception();
}

void my_free_hook(void *ptr, const void *caller) {
	throw std::exception();
}

static void my_init_hook(void) {
	TRACE("start");
	save_hooks();
	setup_hooks();
	TRACE("end");
}

/* Override initializing hook from the C library. */
void (*volatile __malloc_initialize_hook) (void)=my_init_hook;

void malloc_disable(void) {
	malloc_allowed=false;
}

void malloc_enable(void) {
	malloc_allowed=true;
}

int main(int argc, char** argv, char** envp) {
	// this allocation should succeed...
	void* p=malloc(200);
	assert(p!=NULL);
	// we are going into the critical phase of the application...
	malloc_disable();
	// this should fail...
	p=malloc(200);
	// we should never get to this line
	assert(p!=NULL);
	return EXIT_SUCCESS;
}
