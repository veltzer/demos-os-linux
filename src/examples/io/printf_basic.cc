/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This demo shows how to print an unsigned long in a big hexa format...
 */

int main(int argc, char** argv, char** envp) {
	unsigned long i=0xabcd0123;
	void* p=(void *)i;
	char* cp=(char*)i;

	printf("void* printed as '%%p' is %p\n", p);
	printf("char* (and any other pointer) printed as '%%p' is %p\n", cp);
	printf("'%%lx' is %lx\n", i);
	printf("'%%lX' is %lX\n", i);
	printf("'%%#lx' is %#lx\n", i);
	printf("'%%#lX' is %#lX\n", i);
	printf("with conversion '%%lX' is %lX\n", (unsigned long)p);
	int width=5;
	int num=4;
	printf("printing with non compile time width %*d\n", width, num);
	return EXIT_SUCCESS;
}
