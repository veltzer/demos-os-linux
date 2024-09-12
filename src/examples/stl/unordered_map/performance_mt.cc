#include <unordered_map>
#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <vector>

std::unordered_map<int, int> hashMap;
std::mutex mapMutex;

void insertElements(int start, int end) {
    for (int i = start; i < end; ++i) {
        std::lock_guard<std::mutex> lock(mapMutex);
        hashMap[i] = i * 10;
    }
}

void lookupElements(int start, int end) {
    for (int i = start; i < end; ++i) {
        std::lock_guard<std::mutex> lock(mapMutex);
        auto __attribute__((unused)) it = hashMap.find(i);
    }
}

int main() {
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
    std::cout << "STL unordered_map insert time (multi-threaded): " << duration << "ms" << std::endl;

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
    std::cout << "STL unordered_map lookup time (multi-threaded): " << duration << "ms" << std::endl;

    return 0;
}

