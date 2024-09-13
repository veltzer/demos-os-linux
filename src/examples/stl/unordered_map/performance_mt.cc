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

#include <firstinclude.h>
#include <unordered_map>
#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <vector>

using namespace std;

unordered_map<int, int> hashMap;
mutex mapMutex;

void insertElements(int start, int end) {
	for(int i = start; i < end; ++i) {
		lock_guard<mutex> lock(mapMutex);
		hashMap[i] = i * 10;
	}
}

void lookupElements(int start, int end) {
	for(int i = start; i < end; ++i) {
		lock_guard<mutex> lock(mapMutex);
		auto __attribute__((unused)) it = hashMap.find(i);
	}
}

int main() {
	int num_threads = 4;
	int num_elements = 1000000;
	int elements_per_thread = num_elements / num_threads;

	vector<thread> threads;

	// Insertion with multiple threads
	auto start_time = chrono::high_resolution_clock::now();
	for(int i = 0; i < num_threads; ++i) {
		threads.emplace_back(insertElements, i * elements_per_thread, (i + 1) * elements_per_thread);
	}
	for(auto& t : threads) {
		t.join();
	}
	auto end_time = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
	cout << "STL unordered_map insert time (multi-threaded): " << duration << "ms" << endl;

	threads.clear();

	// Lookup with multiple threads
	start_time = chrono::high_resolution_clock::now();
	for(int i = 0; i < num_threads; ++i) {
		threads.emplace_back(lookupElements, i * elements_per_thread, (i + 1) * elements_per_thread);
	}
	for(auto& t : threads) {
		t.join();
	}
	end_time = chrono::high_resolution_clock::now();
	duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
	cout << "STL unordered_map lookup time (multi-threaded): " << duration << "ms" << endl;

	return 0;
}
