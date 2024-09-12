#include <unordered_map>
#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>

std::unordered_map<int, int> hashMap;
std::mutex mapMutex;

void insertElements() {
    for (int i = 0; i < 1000000; ++i) {
        std::lock_guard<std::mutex> lock(mapMutex);
        hashMap[i] = i * 10;
    }
}

void lookupElements() {
    for (int i = 0; i < 1000000; ++i) {
        std::lock_guard<std::mutex> lock(mapMutex);
        auto __attribute__((unused)) it = hashMap.find(i);
    }
}

int main() {
    auto start_time = std::chrono::high_resolution_clock::now();
    // Insert elements
    insertElements();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "STL unordered_map insert time: " << duration << "ms" << std::endl;

    // Lookup elements
    start_time = std::chrono::high_resolution_clock::now();
    lookupElements();
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "STL unordered_map lookup time: " << duration << "ms" << std::endl;

    return 0;
}

