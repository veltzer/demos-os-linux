/*
 * This file is part of the demos-os-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <unordered_map>
#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>

using namespace std;

/*
 * This example benchmarks the performance of STL unordered_map operations
 * in a single-threaded environment with mutex protection. It measures the
 * time taken to insert and lookup one million elements in the hash map.
 * The mutex ensures thread safety but adds overhead even in single-threaded
 * execution. This serves as a baseline for comparing multi-threaded performance.
 */

unordered_map<int, int> hashMap;
mutex mapMutex;

void insertElements() {
	for(int i = 0; i < 1000000; ++i) {
		lock_guard<mutex> lock(mapMutex);
		hashMap[i] = i * 10;
	}
}

void lookupElements() {
	for(int i = 0; i < 1000000; ++i) {
		lock_guard<mutex> lock(mapMutex);
		auto __attribute__((unused)) it = hashMap.find(i);
	}
}

int main() {
	auto start_time = chrono::high_resolution_clock::now();
	// Insert elements
	insertElements();
	auto end_time = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
	cout << "STL unordered_map insert time: " << duration << "ms" << endl;

	// Lookup elements
	start_time = chrono::high_resolution_clock::now();
	lookupElements();
	end_time = chrono::high_resolution_clock::now();
	duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
	cout << "STL unordered_map lookup time: " << duration << "ms" << endl;

	return EXIT_SUCCESS;
}

