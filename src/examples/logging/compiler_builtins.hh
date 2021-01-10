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

#ifndef __compiler_builtins_h
#define __compiler_builtins_h

#include <firstinclude.h>

inline void print_builtins() {
	printf("__BASE_FILE__ is %s\n", __BASE_FILE__);
	printf("__FILE__ is %s\n", __FILE__);
	printf("__LINE__ is %d\n", __LINE__);
	printf("__func__ is %s\n", __func__);
	printf("__FUNCTION__ is %s\n", __FUNCTION__);
	printf("__PRETTY_FUNCTION__ is %s\n", __PRETTY_FUNCTION__);
}
// even this will NOT cause the printing to be of the caller...
void print_builtins() __attribute__((always_inline));
void print_builtins() __attribute__((flatten));
// void print_builtins() __attribute__((artificial));

#define PRINT_BUILTINS() \
	printf("__BASE_FILE__ is %s\n", __BASE_FILE__);	\
	printf("__FILE__ is %s\n", __FILE__); \
	printf("__LINE__ is %d\n", __LINE__); \
	printf("__func__ is %s\n", __func__); \
	printf("__FUNCTION__ is %s\n", __FUNCTION__); \
	printf("__PRETTY_FUNCTION__ is %s\n", __PRETTY_FUNCTION__)

inline void print_from_header() {
	PRINT_BUILTINS();
}

#endif	/* !__compiler_builtins_h */
