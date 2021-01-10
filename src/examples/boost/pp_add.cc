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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <boost/preprocessor/arithmetic/add.hpp>	// for BOOST_PP_ADD

/*
 * This example shows how to use the boost preprocessor library to add
 * integers using the preprocessor.
 *
 * References:
 * http://www.boost.org/doc/libs/1_47_0/libs/preprocessor/doc/ref/add.html
 */

int main(int argc, char** argv, char** envp) {
#define A 14
#define B 26
	printf("did you know that %d+%d=%d\n", A, B, BOOST_PP_ADD(A, B));
	return EXIT_SUCCESS;
}
