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
#include <vector>	// for list<T>, list<T>::iterator
#include <cstdlib>	// for EXIT_SUCCESS, EXIT_FAILURE;

using namespace std;

/*
 * This example demonstrates what happens when you access a vector element
 * out of bounds using the [] operator. Unlike vector.at(), the [] operator
 * does not perform bounds checking, leading to undefined behavior when
 * accessing invalid indices. This program accesses index 7 when the vector
 * only has 2 elements, which may crash or return garbage data.
 */

int main() {
	vector<int> v;
	v.push_back(5);
	v.push_back(6);
	cout << v[7] << endl;
	return EXIT_SUCCESS;
}
