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

#include <urcu.h>
#include <urcu/rculist.h>
#include <cds/urcu/general_instant.h>
#include <cds/container/lazy_list_rcu.h>
#include <cds/container/feldman_hashmap_rcu.h>
#include <cds/init.h>
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

// Define a hash map using Michael's hash map from liburcu
struct KeyVal {
	int key;
	int val;
};

/*
 * EXTRA_LINK_FLAGS_AFTER=-lcds
 */

int main() {
	// Initialize liburcu
	cds::Initialize();
	{
		// Create an RCU type
		typedef cds::urcu::gc< cds::urcu::general_instant<> > rcu_type;
		// Declare the hash table type
		typedef cds::container::FeldmanHashMap<rcu_type, int, int> hash_map_type;

		// Create the hash map
		hash_map_type hashMap;

		auto start_time = chrono::high_resolution_clock::now();
		// Insert elements
		for(int i = 0; i < 1000000; ++i) {
			hashMap.insert(i, i * 10);
		}
		auto end_time = chrono::high_resolution_clock::now();
		auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
		cout << "URCU hash table insert time: " << duration << "ms" << endl;

		// Lookup elements
		start_time = chrono::high_resolution_clock::now();
		for(int i = 0; i < 1000000; ++i) {
			auto __attribute__((unused)) it = hashMap.find(i, [](pair<const int, int>&) {
			// You can process the found value here
			// For simplicity, we don't need to do anything in this example
		});
	}
		end_time = chrono::high_resolution_clock::now();
		duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
		cout << "URCU hash table lookup time: " << duration << "ms" << endl;
	}
	cds::Terminate();
	return EXIT_SUCCESS;
}

