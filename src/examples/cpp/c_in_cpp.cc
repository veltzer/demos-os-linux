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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <multiproc_utils.h>	// for my_system()

/*
 * This example shows how you can write C functions in C++ code.
 * This seems trivial, right ?!? Just write regular functions
 * like:
 * ============SNIP==========
 * int add(int a,int b) {
 *	return a+b;
 * }
 * ============SNIP==========
 * But this is not what I mean....
 * I want the linkage to be C style...
 *
 * The assumption in this example is that it is compiled using a C++
 * compiler...
 */

/*
 * The class here is just to make sure that this code does not get compiled by a C
 * compiler but rather by a C++ compiler.
 *
 * TODO:
 * - Move to checking this using #ifdef cplusplus and #error.
 */
class A{
public:
	int a;
};

// this is a function of C++ code linked in C++ style
int cpp_style_add(int a, int b) {
	return a + b;
}

// this is a function of C++ code linked in C style
// notice that you can still write C code in this function...
extern "C" int c_style_add(int a, int b) {
	// the next line will produce an error
	A *obj=new A();

	obj->a=0;
	int return_value = a + b + obj->a;
	return return_value;
}

int main() {
	printf("These are the symbols without demangling:\n");
	my_system("nm ./c_in_cpp | grep style_add");
	printf("These are the symbols with demangling:\n");
	my_system("nm -C ./c_in_cpp | grep style_add");
	return EXIT_SUCCESS;
}
