/*
 * This file is part of the demos-os-linux package.
 * Copyright (C) 2011-2026 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-os-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-os-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-os-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <cstdlib>	// for EXIT_SUCCESS
#include <iostream>	// for cout, endl
#include <future>	// for future

using namespace std;

/*
 * This example shows that you get a free copy constructor for any class you write
 * (under certain conditions).
 */

class Foo{
private:
	int a;

public:
	Foo(int ia) : a(ia) {
	}
	void print() {
		cout << a << endl;
	}
};

int main() {
	Foo f(5);
	f.print();
	Foo g(f);
	g.print();
	return EXIT_SUCCESS;
}
