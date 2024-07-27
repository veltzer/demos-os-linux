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
#include <utility>
#include <vector>
#include <string>
#include <cstdlib>

using namespace std;

class MyClass{
public:
	MyClass(string str) : data(move(str)) {
	}
	MyClass(const MyClass& other) : data(other.data) {
		cout << "Copy constructor called" << endl;
	}
	MyClass(MyClass&& other) : data(move(other.data)) {
		cout << "Move constructor called" << endl;
	}
	~MyClass() {
		cout << "Destructor called for: " << data << endl;
	}

private:
	string data;
};

int main() {
	string str = "Hello";
	vector<MyClass> vec;

	// Copy construction
	vec.push_back(MyClass(str));
	cout << "After copy construction" << endl;

	// Move construction
	vec.push_back(MyClass(move(str)));
	cout << "After move construction" << endl;

	cout << "Vector size: " << vec.size() << endl;

	return EXIT_SUCCESS;
}
