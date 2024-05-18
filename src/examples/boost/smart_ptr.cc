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
#include <boost/smart_ptr.hpp>	// for boost::scoped_ptr<T>()

/*
 * An example to show boost smart pointers.
 *
 * Notes:
 * - notice that the destructor of A gets called even thought there
 * is no explicit 'delete' in the code (that is the whole point of
 * smart pointers...)
 * - notice that we did not have to link with any boost libraries:
 * the smart_ptr is all in the header files.
 *
 * References:
 * http://www.codeproject.com/Articles/8394/Smart-Pointers-to-boost-your-code#The%20first:%20boost::scoped_ptrT
 *
 */

class A{
public:
	A() {
		std::cout << "in constructor" << std::endl;
	}
	~A() {
		std::cout << "in destructor" << std::endl;
	}
	void doit() {
		std::cout << "doing something" << std::endl;
	}
};

int main() {
	boost::scoped_ptr<A> a(new A);
	a->doit();
	return EXIT_SUCCESS;
}
