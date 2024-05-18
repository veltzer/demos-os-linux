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
#include <list>	// for std::list<T>, std::list<T>::iterator
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * Example of using an stl linked list
 * No need to link with any third party library (STL is just headers...)
 *
 * References:
 * http://www.yolinux.com/TUTORIALS/LinuxTutorialC++STL.html#LIST
 */

int main() {
	std::list<int> l;
	std::cout << l.size() << std::endl;
	l.push_back(0);	// Insert a new element at the end
	l.push_front(0);// Insert a new element at the beginning
	l.insert(++l.begin(), 2);	// Insert "2" before position of first argument
	// (Place before second argument)
	l.push_back(5);
	l.push_back(6);

	int s=l.back();
	l.pop_back();
	std::cout << s << std::endl;

	// lets iterate and print
	std::list<int>::iterator i;
	for(i=l.begin(); i!=l.end(); i++)
		std::cout << *i << " ";
	std::cout << std::endl;
	return EXIT_SUCCESS;
}
