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

/*
 * This example shows how to create an ostream object around standard output
 */

#include <firstinclude.h>
#include <cstdlib>
#include <unistd.h>
#include <ext/stdio_filebuf.h>
#include <iostream>

using namespace std;

int main() {
	cout << "Hello, World!" << endl;
	cout.flush();
	__gnu_cxx::stdio_filebuf<char> filebuf(STDOUT_FILENO, ios::out);
	ostream* os=new ostream(&filebuf);
	*os << "And hello from me too" << endl;
	os->flush();
	delete os;
	cout << "Hello, World! (2nd time)" << endl;
	return EXIT_SUCCESS;
}
