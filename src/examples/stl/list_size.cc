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
#include <measure.h>	// for measure, measure_init(), measure_start(), measure_end(), measure_print()

/*
 * Example showing that list.size() in STL is O(N)!
 *
 * Notes:
 * - there is a claim by one of the participants in the blog that in C++11
 * list.size() is const time. This example is compiled with C++11 support
 * and clearly shows that this is not the case.
 *
 * References:
 * http://www.linuxprogrammingblog.com/std-list-size-complexity
 *
 * EXTRA_COMPILE_FLAGS=-std=c++11
 */

int main() {
	std::list<int> l;
	const unsigned int size_of_list=1000000;
	const unsigned int attempts=1000;
	for(unsigned int i = 0; i < size_of_list; i++)
		l.push_back (i);
	std::cout << "size of list is " << size_of_list << std::endl;
	measure m;
	measure_init(&m, "one l.size()", attempts);
	measure_start(&m);
	volatile __attribute__((unused)) int s;
	for(unsigned int i = 0; i < attempts; i++) {
		s=l.size();
	}
	measure_end(&m);
	measure_print(&m);
	return EXIT_SUCCESS;
}
