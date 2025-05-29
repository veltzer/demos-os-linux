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
 * This example is here to show how to idiomatically pass a variable by references
 * to a c++ function using a reference.
 * Whether the reference should be tagged as "const" or not depends on whether
 * the function will modify the variable or not
 *
 * References
 * - https://www3.ntu.edu.sg/home/ehchua/programming/cpp/cp4_PointerReference.html
 */
#include <firstinclude.h>
#include <iostream>
using namespace std;

void square(int &);

int main() {
	int number = 8;
	cout << "In main: " << &number << endl;	// 0x22ff1c
	cout << number << endl;	// 8
	square(number);	// Implicit referencing (without '&')
	cout << number << endl;	// 64
	return EXIT_SUCCESS;
}

void square(int & rNumber) {	// Function takes an int reference (non-const)
	cout << "In square: " << &rNumber << endl;	// 0x22ff1c
	rNumber *= rNumber;	// Implicit de-referencing (without '*')
}
