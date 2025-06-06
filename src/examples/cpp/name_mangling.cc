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

/*
 * This exampe shows how to demangle names on the command line using
 * nm
 */

#include <firstinclude.h>
#include <iostream>
#include <cstdlib>
#include <multiproc_utils.h>

using namespace std;

class A{
public:
	void doit();
};

void A::doit() {
	cout << "Hello, World!" << endl;
}

int main(int, char** argv) {
	A a;
	a.doit();
	my_system("nm -C %s | grep A::", argv[0]);
	return EXIT_SUCCESS;
}
