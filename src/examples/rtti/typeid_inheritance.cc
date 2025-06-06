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
#include <typeinfo>	// for typeid
#include <cstdlib>	// for EXIT_SUCCESS, srandom(3), random(3)
#include <sys/types.h>	// for getpid(2)
#include <unistd.h>	// for getpid(2)

using namespace std;

/*
 * This example explores static vs dynamic typeid resolution in C++.
 * Note that the last resolution in this example must be dynamic.
 */

// empty class - don't touch this.
class Empty{
};

class Person{
public:
	// ... Person members ...
	virtual~Person() {
	}
};

class Employee: public Person {
	// ... Employee members ...
};

int main() {
	Person person;
	Employee employee;
	Person* ptr;
	srandom(getpid());
	if(random()%10<5) {
		ptr=&employee;
	} else {
		ptr=&person;
	}
	// lets start by printing the sizes of the classes involved...
	cout << "sizeof(Empty) is " << sizeof(Empty) << endl;
	cout << "sizeof(Person) is " << sizeof(Person) << endl;
	cout << "sizeof(Employee) is " << sizeof(Employee) << endl;
	// The string returned by typeid::name() is implementation-defined
	// Person (statically known at compile-time)
	cout << typeid(person).name() << endl;
	// Employee (statically known at compile-time)
	cout << typeid(employee).name() << endl;
	// Person * (statically known at compile-time)
	cout << typeid(ptr).name() << endl;
	// ?? (looked up dynamically at run-time because it is the dereference of a pointer to a polymorphic class)
	cout << typeid(*ptr).name() << endl;
	return EXIT_SUCCESS;
}
