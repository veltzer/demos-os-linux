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

class MyClass {
public:
	MyClass(std::string str) : data(std::move(str)) {}
	MyClass(const MyClass& other) : data(other.data) {
		std::cout << "Copy constructor called" << std::endl;
	}
	MyClass(MyClass&& other) : data(std::move(other.data)) {
		std::cout << "Move constructor called" << std::endl;
	}
	~MyClass() {
		std::cout << "Destructor called for: " << data << std::endl;
	}

private:
	std::string data;
};

int main() {
	std::string str = "Hello";
	std::vector<MyClass> vec;

	// Copy construction
	vec.push_back(MyClass(str));
	std::cout << "After copy construction" << std::endl;

	// Move construction
	vec.push_back(MyClass(std::move(str)));
	std::cout << "After move construction" << std::endl;

	std::cout << "Vector size: " << vec.size() << std::endl;

	return EXIT_SUCCESS;
}
