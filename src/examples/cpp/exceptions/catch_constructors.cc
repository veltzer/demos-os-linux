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
#include <cstdlib>	// for EXIT_SUCCESS
#include <iostream>	// for cerr, endl

using namespace std;

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

class B{
public:
	B() {
		cerr << "In B constructor" << endl;
		throw 20;
	}
};

class A{
private:
	B b;

public:
	A() try: b() {
		cerr << "In A constructor - you won't see this..." << endl;
	} catch (int e) {
		cerr << "got the exception in A constructor" << endl;
	}
};

int main() {
	try {
		A a;
	} catch (int e) {
		cerr << "got the exception in the caller" << endl;
	}
	return EXIT_SUCCESS;
}
