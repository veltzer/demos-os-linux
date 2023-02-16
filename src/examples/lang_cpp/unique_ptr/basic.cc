/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <memory>	// for std::unique_ptr<T>
#include <stdlib.h>	// for EXIT_SUCCESS

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

class A {
public:
	A() {
		std::cout << "in constructor" << std::endl;
	}
	~A() {
		std::cout << "in destructor" << std::endl;
	}
	void doit() {
		std::cout << "in doit" << std::endl;
	}
};

int main(int argc, char** argv, char** envp) {
	std::unique_ptr<A> a(new A);
	a->doit();
	return EXIT_SUCCESS;
}
