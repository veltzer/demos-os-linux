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

#include <firstinclude.h>
#include <stdarg.h>	// for va_start(3), va_end(3), va_list(3)
#include <stdio.h>	// for vfprintf(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * An exampple of using the compiler to inforce 'printf' like correct
 * argument type passing...
 */

void my_trace(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

void my_trace(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
}

int main(int argc, char** argv, char** envp) {
	// this will not compile
	// my_trace("2+2 is %d\n","4");
	// this one does compile
	my_trace("2+2 is %d\n", 4);
	return EXIT_SUCCESS;
}
