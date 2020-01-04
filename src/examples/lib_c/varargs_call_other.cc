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
#include <stdio.h>	// for vprintf(3)
#include <stdarg.h>	// for va_start(3), va_arg(3), va_end(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example shows how to use the glibc varargs support to write a var
 * args function that calls another varargs function.
 *
 * Notes:
 * - we use the program_invocation_short_name which is an external name of
 * our own program. much like argv[0] only we don't need to pass argv until
 * the point we wish to use it.
 * - unlike the corresponding builtin va_* compiler internals we can use this
 * code on any system conforming to the 'stdarg' standard which means this
 * is quite portable.
 * - We use the __attribute__ feature to make sure the compiler checks that
 * we are passing the arguments right (gcc feature).
 */
int trace(const char *fmt, ...) {
	extern char *program_invocation_short_name;

	int ret=printf("%s: ", program_invocation_short_name);
	va_list ap;
	va_start(ap, fmt);
	ret+=vprintf(fmt, ap);
	va_end(ap);
	return(ret);
}

int trace(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

int main(int argc, char** argv, char** envp) {
	trace("%s %d %f\n", "Hello", 5, 3.14);
	// the next line will produce a compile time error (passing int as string...)
	// trace("%s %s %f\n","Hello",5,3.14);
	return EXIT_SUCCESS;
}
