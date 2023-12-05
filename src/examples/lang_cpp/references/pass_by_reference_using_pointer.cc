/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>
using namespace std;

void square(int *);

int main(int argc, char** argv, char** envp) {
	int number = 8;
	cout << "In main: " << &number << endl; // 0x22ff1c
	cout << number << endl; // 8
	square(&number); // Explicit referencing to pass an address
	cout << number << endl; // 64
	return EXIT_SUCCESS;
}

void square(int * pNumber) { // Function takes an int pointer (non-const)
	cout << "In square: " << pNumber << endl; // 0x22ff1c
	*pNumber *= *pNumber; // Explicit de-referencing to get the value pointed-to
}
