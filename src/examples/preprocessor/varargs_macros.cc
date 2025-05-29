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
#include <stdio.h>	// for printf(3), for fprintf(3), stderr:object
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example shows how to create "varargs" macros using the preprocessor.
 *
 * References:
 * https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
 */

#define DEBUG

#ifdef DEBUG
#define TRACE(fmt, args ...) do { printf(fmt, ## args); } while(0)
#else
#define TRACE(fmt, args ...) do { } while(0)
#endif	// DEBUG

#define INFO(...) fprintf(stderr, __VA_ARGS__)
// this cannot be called without extra arguments
#define WARNING(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
// this can!
#define ERROR(fmt, ...) fprintf(stderr, fmt, ## __VA_ARGS__)

int main() {
	TRACE("TRACE: without args\n");
	TRACE("TRACE: with args (%d)\n", 17);
	INFO("INFO: without args\n");
	INFO("INFO: with args (%d)\n", 17);
	// the next line does not compile
	// WARNING("WARNING: without args\n");
	WARNING("WARNING: with args (%d)\n", 17);
	ERROR("ERROR: without args\n");
	ERROR("ERROR: with args (%d)\n", 17);
	return EXIT_SUCCESS;
}
