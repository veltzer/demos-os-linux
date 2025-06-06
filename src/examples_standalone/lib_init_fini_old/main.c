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
#include <stdio.h> // for fprintf(3), stderr
#include <stdlib.h> // for EXIT_SUCCESS

/*
* This is an empty testing application
*/

extern int foo(int,int);
extern int bar(int,int);

int main(int argc,char** argv,char** envp) {
	fprintf(stderr, "2+3 is %d\n", foo(2,3));
	fprintf(stderr, "2+3 is %d\n", bar(2,3));
	fprintf(stderr, "hello from main\n");
	return EXIT_SUCCESS;
}
