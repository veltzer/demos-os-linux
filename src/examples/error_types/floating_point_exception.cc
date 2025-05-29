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
#include <cstdlib>

using namespace std;

/*
 * This is a simple example of how to create a simple floating point exception.
 * The exception is a ** machine hardware level ** exception which causes the CPU
 * to fault, go to the OS, the OS sends a SIGFPE to your program, you can respond
 */

int main() {
	int i=0;
	// cppcheck-suppress zerodiv
	int j=1 / i;

	cout << j << endl;
	return EXIT_SUCCESS;
}
