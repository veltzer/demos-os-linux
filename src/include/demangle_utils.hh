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

#ifndef __demangle_utils_hh
#define __demangle_utils_hh

#include <firstinclude.h>
#include <cxxabi.h>	// for abi::__cxa_demangle(3)
#include <string.h>	// for strncpy(3)
#include <stdio.h>	// for fprintf(3), snprintf(3)
#include <stdlib.h>	// for malloc(3), abort(3)

/*
 * function to demangle name using the C++ API for doing so
 * There is no C API for demangling since C names do not get
 * mangled.
 */
static inline void error_demangle(char* symbol,
	char* result_name,
	unsigned int max_name,
	char* result_offset,
	unsigned int max_offset) {
	char *begin_name=NULL, *begin_offset=NULL, *end_offset=NULL;
	// find parentheses and +address offset surrounding the mangled name:
	// ./module(function+0x15c) [0x8048a6d]
	for(char *p=symbol; *p; ++p) {
		if (*p=='(') {
			begin_name=p;
		} else if (*p=='+') {
			begin_offset=p;
		} else if (*p==')' && begin_offset) {
			end_offset=p;
			break;
		}
	}
	if (begin_name && begin_offset && end_offset && (begin_name < begin_offset)) {
		*begin_name++='\0';
		*begin_offset++='\0';
		*end_offset='\0';
		// mangled name is now in [begin_name, begin_offset) and caller
		// offset in [begin_offset, end_offset). now apply
		// __cxa_demangle():
		// allocate string which will be filled with the demangled function name
		size_t funcnamesize=256;
		char* funcname=(char *)malloc(funcnamesize);
		int status;
		char* ret=abi::__cxa_demangle(begin_name, funcname, &funcnamesize, &status);
		if (status==0) {
			// use possibly realloc()-ed string
			funcname=ret;
			strncpy(result_name, ret, max_name-1);
			strncpy(result_offset, begin_offset, max_offset-1);
		} else {
			// demangling failed. Output function name as a C function with
			// no arguments.
			snprintf(result_name, max_name, "%s()", begin_name);
			snprintf(result_offset, max_offset, "%s", begin_offset);
			// fprintf(stderr, "error demangle: %s: %s()+%s\n", symbol, begin_name, begin_offset);
		}
	} else {
		// couldn't parse the line? print the whole line.
		fprintf(stderr, "error parsing: %s\n", symbol);
		abort();
	}
}

#endif	/* !__demangle_utils_hh */
