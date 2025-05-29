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
 * This example shows how to pass a variable by pointer.
 * The idea of this example is to contract it with the same example using
 * a reference and not a pointer
 *
 * References:
 * - https://www3.ntu.edu.sg/home/ehchua/programming/cpp/cp4_PointerReference.html
 */
#include <firstinclude.h>
#include <iostream>
#include <stdlib.h>
using namespace std;

void square(int * pNumber) {	// Function takes an int pointer (non-const)
	cout << "In square: " << pNumber << endl;	// 0x22ff1c
	*pNumber *= *pNumber;	// Explicit de-referencing to get the value pointed-to
}

int main() {
	int number = 8;
	cout << "In main: " << &number << endl;	// 0x22ff1c
	cout << number << endl;	// 8
	square(&number);// Explicit referencing to pass an address
	cout << number << endl;	// 64
	return EXIT_SUCCESS;
}
