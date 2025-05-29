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
#include <iostream>

using namespace std;

void foo(
	int x1,
	int x2,
	int x3=3,
	int x4=4,
	int x5=5
	) {
	cout << "x1 is " << x1 << endl;
	cout << "x2 is " << x2 << endl;
	cout << "x3 is " << x3 << endl;
	cout << "x4 is " << x4 << endl;
	cout << "x5 is " << x5 << endl;
	cout << "finished" << endl;
}


int main() {
	foo(5, 6, 7);
	return EXIT_SUCCESS;
}
