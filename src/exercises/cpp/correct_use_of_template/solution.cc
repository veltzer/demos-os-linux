<<<<<<< HEAD
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
=======
#include <cstddef>
#include <iostream>
#include <cstring>

// Generic template sort function using a simple insertion sort algorithm
// Works for any type that supports comparison operators
template<typename type>
void sort(type array[], size_t size) {
    for (size_t i = 1; i < size; ++i) {
        type key = array[i];
        size_t j = i;
        while (j > 0 && array[j - 1] > key) {
            array[j] = array[j - 1];
            --j;
        }
        array[j] = key;
    }
}

// Answer to the question: Is there any point to doing specialization for this template?
//
// Generally NO - specialization is NOT needed because:
// 1. The generic template works for any type that supports the '>' comparison operator
// 2. Types like int, double, char, and user-defined types with operator> all work
// 3. Even C-style strings (char*) can be handled by defining a wrapper or using
//    std::string which has operator> defined
//
// However, specialization COULD be useful in specific cases:
// - For C-strings (char*) if you want strcmp-based comparison instead of pointer comparison
// - For types where a more efficient sorting algorithm exists (e.g., radix sort for integers)
// - For types with special memory considerations

// Example specialization for C-strings (char*) using strcmp
template<>
void sort<const char*>(const char* array[], size_t size) {
    for (size_t i = 1; i < size; ++i) {
        const char* key = array[i];
        size_t j = i;
        while (j > 0 && strcmp(array[j - 1], key) > 0) {
            array[j] = array[j - 1];
            --j;
        }
        array[j] = key;
    }
}

// Helper function to print arrays
template<typename type>
void print_array(const type array[], size_t size) {
    for (size_t i = 0; i < size; ++i) {
        std::cout << array[i];
        if (i < size - 1) std::cout << ", ";
    }
    std::cout << std::endl;
}

int main() {
    // Test with integers
    int int_arr[] = {5, 2, 8, 1, 9, 3};
    size_t int_size = sizeof(int_arr) / sizeof(int_arr[0]);
    std::cout << "Integer array before sort: ";
    print_array(int_arr, int_size);
    sort(int_arr, int_size);
    std::cout << "Integer array after sort:  ";
    print_array(int_arr, int_size);
    std::cout << std::endl;

    // Test with doubles
    double double_arr[] = {3.14, 1.41, 2.71, 0.58, 1.73};
    size_t double_size = sizeof(double_arr) / sizeof(double_arr[0]);
    std::cout << "Double array before sort: ";
    print_array(double_arr, double_size);
    sort(double_arr, double_size);
    std::cout << "Double array after sort:  ";
    print_array(double_arr, double_size);
    std::cout << std::endl;

    // Test with characters
    char char_arr[] = {'z', 'a', 'm', 'b', 'k'};
    size_t char_size = sizeof(char_arr) / sizeof(char_arr[0]);
    std::cout << "Char array before sort: ";
    print_array(char_arr, char_size);
    sort(char_arr, char_size);
    std::cout << "Char array after sort:  ";
    print_array(char_arr, char_size);
    std::cout << std::endl;

    // Test with C-strings (uses specialization)
    const char* str_arr[] = {"banana", "apple", "cherry", "date", "apricot"};
    size_t str_size = sizeof(str_arr) / sizeof(str_arr[0]);
    std::cout << "String array before sort: ";
    print_array(str_arr, str_size);
    sort(str_arr, str_size);
    std::cout << "String array after sort:  ";
    print_array(str_arr, str_size);

    return 0;
>>>>>>> fa279291 (no commit message given)
}
