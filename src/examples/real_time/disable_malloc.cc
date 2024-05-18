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
#include <malloc.h>	// for malloc(3) and the hooks
// #define NDEBUG
#include <assert.h>	// for assert(3)
#include <dlfcn.h>	// for dlsym(3)
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
 */

static bool malloc_allowed=true;
static void* (*orig_malloc)(size_t)=NULL;

void malloc_setup(void) {
	if(orig_malloc==NULL) {
		orig_malloc=(void* (*)(size_t))dlsym(RTLD_NEXT, "malloc");
		assert(orig_malloc!=NULL);
	}
}

void malloc_disable(void) {
	malloc_allowed=false;
}

void malloc_enable(void) {
	malloc_allowed=true;
}

void* malloc(size_t size) {
	/*
	 * We have to have this section here since malloc will be called by various
	 * library initialization functions *EVEN BEFORE* we have a chance to call
	 * the above 'malloc_setup' function
	 */
	if(orig_malloc==NULL) {
		orig_malloc=(void* (*)(size_t))dlsym(RTLD_NEXT, "malloc");
		assert(orig_malloc!=NULL);
	}
	assert(malloc_allowed);
	/*
	 * if(!malloc_allowed) {
	 *      printf("calling malloc illegali!\n");
	 *      return NULL;
	 * }
	 */
	return orig_malloc(size);
}

int main() {
	printf("Setting up our own malloc...\n");
	malloc_setup();
	printf("This allocation should succeed...\n");
	void* p=malloc(200);
	assert(p!=NULL);
	free(p);
	printf("We are going into the critical phase of the application...\n");
	malloc_disable();
	printf("This should fail...\n");
	p=malloc(200);
	// we should never get to this line
	printf("p is %p\n", p);
	assert(p!=NULL);
	free(p);
	return EXIT_SUCCESS;
}
