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
#include <iostream>	// for cout, endl
#include <cstdlib>	// for EXIT_SUCCESS
#include <vector>	// for vector:object
#include <algorithm>	// for transform:function

using namespace std;

/*
 * Examples of lambdas from C++0x standard...
 *
 * EXTRA_COMPILE_FLAGS=-std=c++11
 */

static int a=12;
static int b=4;
static int c=7;

int main() {
	vector<int> v;
	for(int i=0; i<10; i++) {
		v.push_back(i);
	}
	for_each(
		v.begin(),
		v.end(),
		[] (int v) {
			cout << v << " ";
		}
	);
	cout << endl;
	transform(
		v.begin(),
		v.end(),
		v.begin(),
		[] (double x) {
			return x*x*a+x*b+c;
		}
	);
	for_each(
		v.begin(),
		v.end(),
		[] (int v) {
			cout << v << " ";
		}
	);
	cout << endl;
	return EXIT_SUCCESS;
}
