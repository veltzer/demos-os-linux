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
#include <iostream>	// for cout, endl
#include <list>	// for list<T>, list<T>::iterator
#include <cstdlib>	// for EXIT_SUCCESS
#include <measure.h>	// for measure, measure_init(), measure_start(), measure_end(), measure_print()

using namespace std;

/*
 * Example measuring std::list::size() performance.
 *
 * Notes:
 * - Since C++11, std::list::size() is required to be O(1) (constant time).
 * - Prior to C++11, some implementations (notably libstdc++) made it O(N)
 *   as a tradeoff to keep splice() O(1).
 * - This example can be used to verify the O(1) complexity on a modern libc++.
 *
 * References:
 * - https://en.cppreference.com/w/cpp/container/list/size
 * - http://www.linuxprogrammingblog.com/std-list-size-complexity
 *
 * EXTRA_COMPILE_FLAGS_BEFORE=-std=c++20
 */

int main() {
	list<int> l;
	const unsigned int size_of_list=1000000;
	const unsigned int attempts=1000;
	for(unsigned int i = 0; i < size_of_list; i++)
		l.push_back (i);
	cout << "size of list is " << size_of_list << endl;
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
