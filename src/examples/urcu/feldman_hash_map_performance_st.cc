#include <urcu.h>
#include <urcu/rculist.h>
#include <cds/urcu/general_instant.h>
#include <cds/container/lazy_list_rcu.h>
#include <cds/container/feldman_hashmap_rcu.h>
#include <cds/init.h>
#include <iostream>
#include <chrono>
#include <thread>

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

        auto start_time = std::chrono::high_resolution_clock::now();
        // Insert elements
        for (int i = 0; i < 1000000; ++i) {
            hashMap.insert(i, i * 10);
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        std::cout << "URCU hash table insert time: " << duration << "ms" << std::endl;

        // Lookup elements
        start_time = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 1000000; ++i) {
            auto __attribute__((unused)) it = hashMap.find(i, [](std::pair<const int, int>&) {
            // You can process the found value here
            // For simplicity, we don't need to do anything in this example
	    });
	}
        end_time = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        std::cout << "URCU hash table lookup time: " << duration << "ms" << std::endl;
    }
    cds::Terminate();
    return 0;
}

