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
#include <stdlib.h>

using namespace std;

class Base {
public:
	Base() {}
	~Base() { cout << "Base destructor" << endl; }
};

class Derived : public Base {
public:
	Derived() {}
	~Derived() { cout << "Derived destructor" << endl; }
};

int main() {
	Base* b = new Derived();
	delete b; // Only calls Base destructor!
	return EXIT_SUCCESS;
}
