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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <vector>	// for std::vector
#include <algorithm>	// for std::transform

/*
 * An example of using lambda with algorithms
 */

int main(int argc, char** argv, char** envp) {
	std::vector<int> v;
	for(int i=0; i<10; i++) {
		v.push_back(i);
	}
	auto f=[] (double x){ return x*x*12+x*4+7; };
	std::transform(v.begin(), v.end(), v.begin(), f);
	for (int num : v) {
		std::cout << num << " ";
	}
	std::cout << std::endl;
	return EXIT_SUCCESS;
}
