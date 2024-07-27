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
#include <iostream>	// for std::cout, std::endl
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example investigates the sizeof() of C++ object.
 *
 * NOTES:
 * - why is the size of an empty class in C++ 1 and not 0?
 * why is the size of an empty struct in C++ 1 and not 0?
 * this is in contrast to a C struct which, when empty, has size 0.
 * the answer lies in allocating many these structs/instances. If the size of
 * a single struct/instance is 0 then different structs/instances will
 * have the same pointer pointing to them. This conflict with the C++ ==
 * operator which considers two objects which have the same pointer
 * to them as equal. So C++ has philosphical issues that prevent the possibility
 * of having structs/instances of size 0.
 */

class A{
public:
	int a;
	char b;
	int c;
	char d;

	void dosomething() {
		for(int i=0; i<4; i++) {
			std::cout << "Hello from A, i is " << i << std::endl;
		}
	}
	virtual~A() {
	}
};

class B: public A {
public:
	int e;
	char f;
	void dosomething() {
		for(int i=0; i<4; i++) {
			std::cout << "Hello from B, i is " << i << std::endl;
		}
	}
};
#define CppOffsetOf(className, FieldName) ((char *)(&(((className *)1)->FieldName)) - (char *)1)

class C{
};

struct empty{
};

int main() {
	std::cout << "sizeof(A) is " << sizeof(A) << std::endl;
	std::cout << "sizeof(B) is " << sizeof(B) << std::endl;
	std::cout << "sizeof(C) is " << sizeof(C) << std::endl;
	std::cout << "sizeof(struct empty) is " << sizeof(struct empty) << std::endl;
	// std::cout << "__builtin_offsetof(e,B) " << __builtin_offsetof(e,B) << std::endl;
	std::cout << "CppOffsetOf(B,e) " << CppOffsetOf(B, e) << std::endl;
	std::cout << "CppOffsetOf(A,a) " << CppOffsetOf(A, a) << std::endl;
	std::cout << "CppOffsetOf(A,b) " << CppOffsetOf(A, b) << std::endl;
	std::cout << "CppOffsetOf(A,c) " << CppOffsetOf(A, c) << std::endl;
	std::cout << "CppOffsetOf(A,d) " << CppOffsetOf(A, d) << std::endl;

	A* a=new A();
	a->dosomething();
	delete a;

	A* ab=new B();
	ab->dosomething();
	delete ab;

	B* b=new B();
	b->dosomething();
	delete b;

	A anotherA;
	anotherA.dosomething();

	return EXIT_SUCCESS;
}
