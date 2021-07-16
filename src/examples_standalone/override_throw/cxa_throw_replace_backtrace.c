/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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

#define _GNU_SOURCE	// for RTLD_NEXT
#include <dlfcn.h>	// for dlsym(3), RTLD_NEXT
#include <execinfo.h>	// for backtrace(3), backtrace_symbols_fd(3)
#include <stdio.h>	// for stderr:object, fprintf(3)

typedef void (*cxa_throw_type)(void *, void *, void (*) (void *));
// this is automatically initialized to NULL
static cxa_throw_type orig_cxa_throw;
const unsigned int MAX_STACK_SIZE=256;
// also initialized to 0
static int throw_count;

void __cxa_throw(void *thrown_exception, void *pvtinfo, void (*dest)(void *)) {
	void *array[MAX_STACK_SIZE];
	size_t size;
	if (orig_cxa_throw == NULL) {
		orig_cxa_throw = (cxa_throw_type) dlsym(RTLD_NEXT, "__cxa_throw");
	}
	size = backtrace(array, MAX_STACK_SIZE);
	fprintf(stderr, "#### START OF STACK TRACE (#%d) ####\n", ++throw_count);
	backtrace_symbols_fd(array, size, fileno(stderr));
	fprintf(stderr, "#### END OF STACK TRACE ####\n");
	orig_cxa_throw(thrown_exception, pvtinfo, dest);
}
