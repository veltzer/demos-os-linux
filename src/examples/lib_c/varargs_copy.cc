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
#include <stdio.h>	// for printf(3)
#include <stdarg.h>	// for va_start(3), va_arg(3), va_end(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example shows how to use the 'va_copy' function.
 */
void trace(const char *fmt, ...) {
	va_list ap;
	int d;
	char c, *s;
	float f;
	va_start(ap, fmt);
	while(*fmt) {
		switch (*fmt++) {
		case 'f':	/* float */
			f=(double)va_arg(ap, double);
			printf("float %f\n", f);
			break;
		case 's':	/* string */
			s=va_arg(ap, char *);
			printf("string %s\n", s);
			break;
		case 'd':	/* int */
			d=va_arg(ap, int);
			printf("int %d\n", d);
			break;
		case 'c':	/* char */
			/* need a cast here since va_arg only
			 * takes fully promoted types */
			c=(char)va_arg(ap, int);
			printf("char %c\n", c);
			break;
		}
	}
	va_end(ap);
}

void trace(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

int main(int argc, char** argv, char** envp) {
	printf("sizeof(va_list) is [%zd]\n", sizeof(va_list));
	// trace("%s %d %f\n", "Hello", 5, 3.14);
	return EXIT_SUCCESS;
}
