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
#include <vector>	// for std::list<T>, std::list<T>::iterator
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE;

/*
 * This example shows that >> is no longer identified by the compiler as an operator name.
 * This means that you can write:
 * vector<vector<int>> v;
 * whilte before you had to write:
 * vector<vector<int> > v;
 * because >> was identified as an operator.
 */

using namespace std;

int main() {
	vector<vector<int> > v;
	return EXIT_SUCCESS;
}
