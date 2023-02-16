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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <iostream>	// for std::cout, std::endl

/*
 * This is an example of template specialization.
 *
 * References:
 * http://www.cprogramming.com/tutorial/template_specialization.html
 */

template<typename T> class Vector {
private:
	unsigned int size;

public:
	Vector(unsigned int isize) {
		size=isize;
	}
	unsigned int getSize() {
		return size;
	}
};

template<> class Vector<bool> {
private:
	unsigned int size;

public:
	Vector(unsigned int isize) {
		size=isize/8;
	}
	unsigned int getSize() {
		return size;
	}
};

int main(int argc, char** argv, char** envp) {
	Vector<int> v1(100);
	std::cout << "v1.getSize() is " << v1.getSize() << std::endl;
	Vector<bool> v2(100);
	std::cout << "v2.getSize() is " << v2.getSize() << std::endl;
	return EXIT_SUCCESS;
}
