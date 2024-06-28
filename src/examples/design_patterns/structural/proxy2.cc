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
#include <iostream>
#include <string>

using namespace std;

template <typename ANYTYPE>
class POP {			 // Persistent Object Pointer
	string oid;
	ANYTYPE*	ptr;
public:
	POP( string id ) { oid = id; ptr = 0; }
	~POP()				{ delete ptr; }
	ANYTYPE* operator->() {
		if ( ! ptr)
			// simulate the persistence mechanism
			ptr = new ANYTYPE( oid );
		return ptr;
	}
};

class Person {
	string name;
	int	 age;
public:
	Person( string n ) { name = n; }
	string getName()	{ return name; }
	int	 getAge()	 { return 32; }
};

int main() {
	POP<Person> ph( "Tom" );
	cout << "policy holder is " << ph->getName() << ", age is " << ph->getAge() << '\n';
	return 0;
}
