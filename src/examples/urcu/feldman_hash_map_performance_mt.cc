#include <urcu.h>
#include <urcu/rculist.h>
#include <cds/urcu/general_instant.h>
#include <cds/container/lazy_list_rcu.h>
#include <cds/container/feldman_hashmap_rcu.h>
#include <cds/init.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

/*
 * EXTRA_LINK_FLAGS_AFTER=-lcds
 */

typedef cds::urcu::gc< cds::urcu::general_instant<> > rcu_type;
typedef cds::container::FeldmanHashMap<rcu_type, int, int> hash_map_type;

hash_map_type hashMap;

void insertElements(int start, int end) {
    for (int i = start; i < end; ++i) {
        hashMap.insert(i, i * 10);
    }
}

void lookupElements(int start, int end) {
    for (int i = start; i < end; ++i) {
	    auto __attribute__((unused)) it = hashMap.find(i, [](std::pair<const int, int>&) {
            // You can process the found value here
            // For simplicity, we don't need to do anything in this example
            });
    }
}

int main() {
    cds::Initialize();
    {
        rcu_type rcu;

        int num_threads = 4;
        int num_elements = 1000000;
        int elements_per_thread = num_elements / num_threads;

        std::vector<std::thread> threads;

        // Insertion with multiple threads
        auto start_time = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(insertElements, i * elements_per_thread, (i + 1) * elements_per_thread);
        }
        for (auto& t : threads) {
            t.join();
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        std::cout << "URCU hash table insert time (multi-threaded): " << duration << "ms" << std::endl;

        threads.clear();

        // Lookup with multiple threads
        start_time = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(lookupElements, i * elements_per_thread, (i + 1) * elements_per_thread);
        }
        for (auto& t : threads) {
            t.join();
        }
        end_time = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        std::cout << "URCU hash table lookup time (multi-threaded): " << duration << "ms" << std::endl;
    }
    cds::Terminate();
    return 0;
}

