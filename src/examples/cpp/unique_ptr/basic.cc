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
#include <cstdlib>	// for EXIT_SUCCESS
#include <iostream>	// for cout, endl
#include <memory>	// for unique_ptr<T>

using namespace std;

/*
 * This example demonstrates the use of unique_ptr of the standard C++ library.
 * This replaced the old 'auto_ptr' which is now deprecated.
 *
 * Notes:
 * - note that there is no need to call the destructor (that is the whole
 * point of the automatic pointer...)
 *
 * Refernces:
 * - http://en.wikipedia.org/wiki/Auto_ptr
 * - http://en.cppreference.com/w/cpp/memory/unique_ptr
 */

class A{
public:
	A() {
		cout << "in constructor" << endl;
	}
	~A() {
		cout << "in destructor" << endl;
	}
	void doit() {
		cout << "in doit" << endl;
	}
};

int main() {
	unique_ptr<A> a(new A);
	a->doit();
	return EXIT_SUCCESS;
}
