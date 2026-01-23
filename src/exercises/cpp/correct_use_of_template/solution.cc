/*
 * This file is part of the demos-os-linux package.
 * Copyright (C) 2011-2026 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-os-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-os-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-os-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <cstddef>
#include <iostream>
#include <cstring>
#include <stdlib.h>

using namespace std;

// Generic template sort function using a simple selection sort algorithm
template<typename type>
void sort(type array[], size_t size) {
	for(size_t i = 0; i < size; i++) {
		size_t min_idx = i;
		for(size_t j = i + 1; j < size; j++) {
			if (array[j] < array[min_idx]) {
				min_idx = j;
			}
		}
		if (min_idx != i) {
			type temp = array[i];
			array[i] = array[min_idx];
			array[min_idx] = temp;
		}
	}
}

// Specialization for C-style strings (const char*)
// This IS useful because we need strcmp instead of < operator
template<>
void sort<const char*>(const char* array[], size_t size) {
	for(size_t i = 0; i < size; i++) {
		size_t min_idx = i;
		for(size_t j = i + 1; j < size; j++) {
			if (strcmp(array[j], array[min_idx]) < 0) {
				min_idx = j;
			}
		}
		if (min_idx != i) {
			const char* temp = array[i];
			array[i] = array[min_idx];
			array[min_idx] = temp;
		}
	}
}

// Test the implementation
int main() {
	// Test with integers
	int int_array[] = {5, 2, 8, 1, 9, 3};
	size_t int_size = sizeof(int_array) / sizeof(int_array[0]);

	cout << "Integer array before sorting: ";
	for(size_t i = 0; i < int_size; i++) {
		cout << int_array[i] << " ";
	}
	cout << endl;

	sort(int_array, int_size);

	cout << "Integer array after sorting: ";
	for(size_t i = 0; i < int_size; i++) {
		cout << int_array[i] << " ";
	}
	cout << endl;

	// Test with doubles
	double double_array[] = {3.14, 1.41, 2.71, 0.57};
	size_t double_size = sizeof(double_array) / sizeof(double_array[0]);

	cout << "\nDouble array before sorting: ";
	for(size_t i = 0; i < double_size; i++) {
		cout << double_array[i] << " ";
	}
	cout << endl;

	sort(double_array, double_size);

	cout << "Double array after sorting: ";
	for(size_t i = 0; i < double_size; i++) {
		cout << double_array[i] << " ";
	}
	cout << endl;

	// Test with C-style strings (demonstrates specialization)
	const char* str_array[] = {"banana", "apple", "cherry", "date"};
	size_t str_size = sizeof(str_array) / sizeof(str_array[0]);

	cout << "\nString array before sorting: ";
	for(size_t i = 0; i < str_size; i++) {
		cout << str_array[i] << " ";
	}
	cout << endl;

	sort(str_array, str_size);

	cout << "String array after sorting: ";
	for(size_t i = 0; i < str_size; i++) {
		cout << str_array[i] << " ";
	}
	cout << endl;

	return EXIT_SUCCESS;
}
