/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <iostream>	// for std::cout, std::endl
#include <assert.h>	// for assert(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example shows that a single virtual function in an object increases it's size.
 */

// A1 and A2 are exactly the same except the virtual in A2

class A1 {
public:
	int a;

	A1(int val) {
		a=val;
	}
	void dosomething() {
		for(int i=0; i<4; i++) {
			std::cout << "Hello from A, i is " << i << std::endl;
		}
	}
};

class A2 {
public:
	int a;

	A2(int val) {
		a=val;
	}

	virtual void dosomething() {
		for(int i=0; i<4; i++) {
			std::cout << "Hello from A, i is " << i << std::endl;
		}
	}
};

int main(int argc, char** argv, char** envp) {
	std::cout << "sizeof(A1) is " << sizeof(A1) << std::endl;
	std::cout << "sizeof(A2) is " << sizeof(A2) << std::endl;
	A2 obj1(4), obj2(5);
	// this is the vtable
	void** vp1=(void**)&obj1;
	void** vp2=(void**)&obj2;
	assert(*vp1==*vp2);
	// this is where the data is
	int* p1=(int*)&obj1;
	int* p2=(int*)&obj2;
	assert(4==*(p1+1));
	assert(5==*(p2+1));
	return EXIT_SUCCESS;
}
