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
#include <iostream>	// for cout, endl
#include <future>	// for future

using namespace std;

/*
 * This example shows that you get a free constructor for any class you write
 * which initializes it's data to 0.
 * (under certain conditions).
 */

class Foo{
private:
	int n=0;
	void* p=NULL;

public:
	void print() {
		cout << "n is " << n << endl;
		cout << "p is " << hex << showbase << p << endl;
	}
};

int main() {
	Foo f;
	f.print();
	return EXIT_SUCCESS;
}
