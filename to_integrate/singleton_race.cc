#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <random>
#include <set>
#include <mutex>

class Singleton {
private:
    static Singleton* instance;
    int value;
    
    // Private constructor to prevent direct instantiation
    Singleton() {
        // Simulate some initialization work
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        // Random value for demonstration
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 1000);
        value = dis(gen);
        
        std::cout << "Created instance at address: " << this 
                  << " with value: " << value << std::endl;
    }
    
public:
    // Classic singleton getter with RACE CONDITION
    static Singleton* getInstance() {
        // THE RACE CONDITION IS HERE:
        // Multiple threads can pass this check simultaneously
        if (instance == nullptr) {
            // Simulate some work that makes race condition more likely
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            
            // Multiple threads can reach this point and each create an instance
            instance = new Singleton();
        }
        return instance;
    }
    
    int getValue() const {
        return value;
    }
    
    // Prevent copying
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    
    // For demonstration - normally you wouldn't have this
    void* getAddress() const {
        return (void*)this;
    }
    
    // Add a reset method for testing (normally you wouldn't have this)
    static void reset() {
        delete instance;
        instance = nullptr;
    }
}; Multiple threads can reach this point and each create an instance
            instance = new Singleton();
        }
        return instance;
    }
    
    int getValue() const {
        return value;
    }
    
    // Prevent copying
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    
    // For demonstration - normally you wouldn't have this
    void* getAddress() const {
        return (void*)this;
    }
};

// Static member definition
Singleton* Singleton::instance = nullptr;

// Function to be run by each thread
void createSingleton(int threadId, std::vector<std::pair<int, void*>>& results, std::mutex& resultsMutex) {
    std::cout << "Thread " << threadId << " starting..." << std::endl;
    
    Singleton* singleton = Singleton::getInstance();
    
    std::cout << "Thread " << threadId << " got instance at: " << singleton->getAddress() 
              << " with value: " << singleton->getValue() << std::endl;
    
    // Store results thread-safely
    std::lock_guard<std::mutex> lock(resultsMutex);
    results.push_back({threadId, singleton->getAddress()});
}

int main() {
    std::cout << "=== Demonstrating C++ Singleton Race Condition ===\n" << std::endl;
    
    // Note: We can't reset the singleton from outside since instance is private
    // This demonstrates a real-world scenario where the singleton persists
    
    const int NUM_THREADS = 5;
    std::vector<std::thread> threads;
    std::vector<std::pair<int, void*>> results;
    std::mutex resultsMutex;
    
    // Create and start multiple threads simultaneously
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(createSingleton, i, std::ref(results), std::ref(resultsMutex));
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Analyze results
    std::cout << "\n=== Results ===" << std::endl;
    std::set<void*> uniqueInstances;
    
    for (const auto& result : results) {
        std::cout << "Thread " << result.first << ": Instance at " << result.second << std::endl;
        uniqueInstances.insert(result.second);
    }
    
    std::cout << "\nTotal unique instances created: " << uniqueInstances.size() << std::endl;
    
    if (uniqueInstances.size() > 1) {
        std::cout << "❌ RACE CONDITION DETECTED! Multiple instances were created." << std::endl;
    } else {
        std::cout << "✅ No race condition occurred this time (try running again)." << std::endl;
    }
    
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "Run this program multiple times to see the race condition!" << std::endl;
    std::cout << "The number of instances created may vary between runs." << std::endl;
    
    return 0;
}

/*
THREAD-SAFE SOLUTIONS:

1. Double-Checked Locking (correct implementation):
class ThreadSafeSingleton {
private:
    static ThreadSafeSingleton* instance;
    static std::mutex mtx;
    
public:
    static ThreadSafeSingleton* getInstance() {
        if (instance == nullptr) {
            std::lock_guard<std::mutex> lock(mtx);
            if (instance == nullptr) {  // Double-check with lock
                instance = new ThreadSafeSingleton();
            }
        }
        return instance;
    }
};

2. Meyer's Singleton (C++11 thread-safe):
class MeyersSingleton {
public:
    static MeyersSingleton& getInstance() {
        static MeyersSingleton instance;  // Thread-safe in C++11+
        return instance;
    }
private:
    MeyersSingleton() = default;
};

3. std::once_flag approach:
class OnceFlagSingleton {
private:
    static OnceFlagSingleton* instance;
    static std::once_flag flag;
    
public:
    static OnceFlagSingleton* getInstance() {
        std::call_once(flag, []() {
            instance = new OnceFlagSingleton();
        });
        return instance;
    }
};
*/