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

#include <firstinclude.h>
#include <stdio.h> // for printf(3)
#include <stdlib.h> // for EXIT_SUCCESS

/*
* This is an example shows how to determine an include file at compile time
* in a way which is dependent on compile time macros. You could do this using
* #ifdef but the idea is that you do not want to change this code as you add
* more include files.
*
* Stolen from compiler-gcc.h from the kernel.
*/

#define __quote(x) #x
#define _myheader(y) __quote(header_##y.h)
#define myheader(x) _myheader(x)
#include myheader(TYPE)

int main(int argc,char** argv,char** envp) {
	printf("SYMBOL is %s\n",SYMBOL);
	return EXIT_SUCCESS;
}
