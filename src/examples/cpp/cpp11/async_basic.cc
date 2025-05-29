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
 * This is a basic example of C++11 async tasks.
 *
 * References:
 * http://solarianprogrammer.com/2012/10/17/cpp-11-async-tutorial
 */

int main() {
	// called_from_async launched in a separate thread if possible
	future<int> result(async([](int m, int n) {
		return m + n;
		}, 2, 4));
	cout << "Message from main" << endl;
	// retrive and print the value stored in the future
	cout << result.get() << endl;
	return EXIT_SUCCESS;
}
