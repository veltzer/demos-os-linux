/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2024 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <cstdlib>
#include <iostream>

using namespace std;

/*
 * This is an example of initializing a struct in C++...
 * XTRA_COMPILE_FLAGS=-Wno-error=uninitialized -w
 * EXTRA_COMPILE_FLAGS_AFTER=-std=gnu++20
 */

struct mystruct {
	int a=0;
	int b=1;
};

int main() {
	#pragma GCC diagnostic ignored "-Wpedantic"
	// Constructor initialization
	mystruct v1;
	cout << "v1.a is " << v1.a << endl;
	cout << "v1.b is " << v1.b << endl;
	// uniformd initialization, C++11 and later
	struct mystruct v2 = { 2, 3 };
	cout << "v2.a is " << v2.a << endl;
	cout << "v2.b is " << v2.b << endl;
	// this is C99 style in C++ it is called designated initialization,
	// Supported since C++ 20 in C++
	struct mystruct v3={ .a=4, .b=5 };
	cout << "v3.a is " << v3.a << endl;
	cout << "v3.b is " << v3.b << endl;
	// this is the GNU style...
	struct mystruct v4={ a : 6, b : 7 };
	cout << "v4.a is " << v4.a << endl;
	cout << "v4.b is " << v4.b << endl;
	// member by member, this will always work
	struct mystruct v5;
	v5.a=8;
	v5.b=9;
	cout << "v5.a is " << v5.a << endl;
	cout << "v5.b is " << v5.b << endl;
	return EXIT_SUCCESS;
}
