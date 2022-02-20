/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <us_helper.h>	// for quote(), stringify()

/*
 * This show how to use the preprocessors '#' operator to quote stuff
 *
 * This example shows how anything could be turned into a macro
 * using the C pre-processor.
 */

#define FOO BAR

int main(int argc, char** argv, char** envp) {
	printf("quote(5) is %s\n", quote(5));
	// C/C++ automatically catenates adjacent strings into one large string
	printf("Hello, " quote(World) "\n");
	printf("Hello, " quote(printf("World")) "\n");
	// FOO will not get substituted for BAR in the next line (the pre-processor does not DO
	// simple recursion)
	printf("this is some " quote(FOO) " string\n");
	// but this will work...
	printf("this is some " stringify(FOO) " string\n");
	return EXIT_SUCCESS;
}
