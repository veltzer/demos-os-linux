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
#include <stdio.h>	// for fprintf(3)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <us_helper.h>	// for stringify()
#include <assert.h>	// for assert(3)

/*
 * This example shows how you can build a useful ASSERT macro
 * It also demonstrates the glibc assert(3) library call.
 *
 * Note:
 * - to behave like the builtin 'assert' we dump core at the end using
 * the abort(3) library function.
 * - we try to emulate the library function 'assert' as much as possbile.
 */

#define ASSERT(expr) \
	if(!(expr)) { \
		fprintf(stderr, "%s: %s:%d: %s: Assertion `%s' failed.\n", program_invocation_short_name, __FILE__, __LINE__, __PRETTY_FUNCTION__, stringify(expr)); \
		abort(); \
	}

int main(int argc, char** argv, char** envp) {
	int x=1;
	// ASSERT(x==2);
	assert(x==2);
	return EXIT_SUCCESS;
}
