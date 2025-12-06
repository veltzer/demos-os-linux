/*
 * This file is part of the demos-os-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-os-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-os-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-os-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <iostream>	// for cout, endl
#include <typeinfo>	// for typeid
#include <cstdlib>	// for EXIT_SUCCESS

using namespace std;

/*
 * Demo the typeid function of C++, with RTTI
 */

class A{
};

class B{
public:
	virtual void doit() {
		cout << "Hello, World!" << endl;
	}
};

int main() {
	A a;
	B b;
	cout << typeid(a).name() << endl;
	cout << typeid(b).name() << endl;
	cout << typeid(cout).name() << endl;
	return EXIT_SUCCESS;
}
