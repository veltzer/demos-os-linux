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
#include <stdbool.h>	// for bool, true and false
#include <us_helper.h>	// for stringify()

/*
 * This is a demo of how to use the _Bool and stdbool.h headers for
 * boolean support in C code.
 *
 * Note that this code MUST be compiled using a C compiler and not a C++
 * compiler since the results will be different. In a C++ compiler,
 * _Bool is bool which is a built in type and true and false are built
 * ins.
 *
 * References:
 * http://stackoverflow.com/questions/1608318/is-bool-a-native-c-type
 */

int main(int argc, char** argv, char** envp) {
	printf("sizeof(_Bool) is %zd\n", sizeof(_Bool));
	printf("sizeof(bool) is %zd\n", sizeof(bool));
	printf("_Bool is really %s\n", stringify(_Bool));
	printf("bool is really %s\n", stringify(bool));
	printf("true is really %s\n", stringify(true));
	printf("false is really %s\n", stringify(false));
	return(0);
}
