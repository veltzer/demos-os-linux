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

#include <unordered_map>
#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>

using namespace std;

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

    return 0;
}

