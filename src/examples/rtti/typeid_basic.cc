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
#include <typeinfo>	// for typeid
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * Demo the typeid function of C++, with RTTI
 */

class A {
};

class B {
public:
	virtual void doit() {
		std::cout << "Hello, World!" << std::endl;
	}
};

int main(int argc, char** argv, char** envp) {
	A a;
	B b;
	std::cout << typeid(a).name() << std::endl;
	std::cout << typeid(b).name() << std::endl;
	std::cout << typeid(std::cout).name() << std::endl;
	return EXIT_SUCCESS;
}
