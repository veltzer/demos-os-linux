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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <iostream>	// for std::cerr, std::endl

/*
 * This is an example of catching exceptions from constructors
 *
 * Notes:
 * - You can't handle the exception when using function try blocks on constructors.
 * even if your catch(...) block does not re-throw, the exception still escapes to the caller.
 * - In the caller you can catch the exception and not re-throw it.
 *
 * References:
 * http://stackoverflow.com/questions/160147/catching-exceptions-from-a-constructors-initializer-list
 */

class B {
public:
	B() {
		std::cerr << "In B constructor" << std::endl;
		throw 20;
	}
};

class A {
private:
	B b;

public:
	A() try : b() {
		std::cerr << "In A constructor - you won't see this..." << std::endl;
	} catch (int e) {
		std::cerr << "got the exception in A constructor" << std::endl;
	}
};

int main(int argc, char** argv, char** envp) {
	try {
		A a;
	} catch (int e) {
		std::cerr << "got the exception in the caller" << std::endl;
	}
	return EXIT_SUCCESS;
}
