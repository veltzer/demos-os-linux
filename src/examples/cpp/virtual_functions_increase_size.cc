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
#include <iostream>	// for cout, endl
#include <cassert>	// for assert()
#include <cstdlib>	// for EXIT_SUCCESS
#include <cstdint>	// for intptr_t

using namespace std;

/*
 * This example shows that a single virtual function in an object increases it's size.
 */

// A1 and A2 are exactly the same except the virtual in A2

class A1{
public:
	intptr_t a;

	A1(int val) : a(val) {
	}
	void dosomething() {
		cout << "Hello from A1" << endl;
	}
};

class A2{
public:
	intptr_t a;

	A2(int val) : a(val) {
	}

	virtual void dosomething() {
		cout << "Hello from A2" << endl;
	}
};

int main() {
	assert(sizeof(A1)+sizeof(nullptr)==sizeof(A2));
	cout << "sizeof(A1) is " << sizeof(A1) << endl;
	cout << "sizeof(A2) is " << sizeof(A2) << endl;
	A2 obj1(4), obj2(5);
	// this is the vtable
	void** vp1=(void**)&obj1;
	void** vp2=(void**)&obj2;
	assert(*vp1==*vp2);
	// this is where the data is
	intptr_t* p1=(intptr_t*)&obj1;
	intptr_t* p2=(intptr_t*)&obj2;
	assert(4==*(p1+1));
	assert(5==*(p2+1));
	return EXIT_SUCCESS;
}
