#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <chrono>
#include <cassert>

// Configuration
constexpr int NUM_THREADS = 4;
constexpr int WORK_PER_THREAD = 100000;
constexpr int TOTAL_EXPECTED = NUM_THREADS * WORK_PER_THREAD;

// =============================================================================
// Solution 1: BROKEN - No synchronization (shows the problem)
// =============================================================================
class BrokenCounter {
private:
    int counter;  // NO volatile, NO synchronization
    
public:
    BrokenCounter() : counter(0) {}
    
    void increment() {
        counter++;  // Race condition!
    }
    
    int get() const { return counter; }
    const char* name() const { return "Broken (no sync)"; }
};

// =============================================================================
// Solution 2: STILL BROKEN - Volatile only (shows volatile isn't enough)
// =============================================================================
class VolatileOnlyCounter {
private:
    volatile int counter;  // volatile but no atomicity
    
public:
    VolatileOnlyCounter() : counter(0) {}
    
    void increment() {
        counter++;  // Still a race condition! Read-modify-write not atomic
    }
    
    int get() const { return counter; }
    const char* name() const { return "Volatile only (still broken)"; }
};

// =============================================================================
// Solution 3: CORRECT - Mutex protection
// =============================================================================
class MutexCounter {
private:
    int counter;
    mutable std::mutex mtx;
    
public:
    MutexCounter() : counter(0) {}
    
    void increment() {
        std::lock_guard<std::mutex> lock(mtx);
        counter++;
    }
    
    int get() const {
        std::lock_guard<std::mutex> lock(mtx);
        return counter;
    }
    
    const char* name() const { return "Mutex protected"; }
};

// =============================================================================
// Solution 4: CORRECT - Atomic operations
// =============================================================================
class AtomicCounter {
private:
    std::atomic<int> counter;
    
public:
    AtomicCounter() : counter(0) {}
    
    void increment() {
        counter++;  // Atomic increment
        // Equivalent to: counter.fetch_add(1);
    }
    
    int get() const {
        return counter.load();
    }
    
    const char* name() const { return "Atomic operations"; }
};

// =============================================================================
// Solution 5: CORRECT - Atomic with memory ordering
// =============================================================================
class AtomicRelaxedCounter {
private:
    std::atomic<int> counter;
    
public:
    AtomicRelaxedCounter() : counter(0) {}
    
    void increment() {
        counter.fetch_add(1, std::memory_order_relaxed);
    }
    
    int get() const {
        return counter.load(std::memory_order_relaxed);
    }
    
    const char* name() const { return "Atomic (relaxed ordering)"; }
};

// =============================================================================
// Solution 6: HYBRID - Volatile + Atomic operations (educational)
// =============================================================================
class VolatileAtomicCounter {
private:
    volatile std::atomic<int> counter;  // Both volatile AND atomic
    
public:
    VolatileAtomicCounter() : counter(0) {}
    
    void increment() {
        counter.fetch_add(1);
    }
    
    int get() const {
        return counter.load();
    }
    
    const char* name() const { return "Volatile + Atomic"; }
};

// =============================================================================
// Worker function template
// =============================================================================
template<typename CounterType>
void worker_thread(CounterType& counter, int work_count) {
    for (int i = 0; i < work_count; ++i) {
        // Simulate some work
        volatile int dummy = 0;
        for (int j = 0; j < 100; ++j) {
            dummy += j;  // Prevent optimization
        }
        
        // Increment counter after "work" is done
        counter.increment();
    }
}

// =============================================================================
// Benchmark function template
// =============================================================================
template<typename CounterType>
void benchmark_counter(const std::string& test_name) {
    std::cout << "\n=== " << test_name << " ===" << std::endl;
    
    CounterType counter;
    std::vector<std::thread> threads;
    
    // Start timing
    auto start = std::chrono::high_resolution_clock::now();
    
    // Create and start threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(worker_thread<CounterType>, 
                           std::ref(counter), WORK_PER_THREAD);
    }
    
    // Wait for all threads to complete
    for (auto& t : threads) {
        t.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Results
    int final_count = counter.get();
    std::cout << "Counter type: " << counter.name() << std::endl;
    std::cout << "Final count: " << final_count 
              << " (expected: " << TOTAL_EXPECTED << ")" << std::endl;
    std::cout << "Time taken: " << duration.count() << " microseconds" << std::endl;
    
    if (final_count == TOTAL_EXPECTED) {
        std::cout << "✅ CORRECT RESULT" << std::endl;
    } else {
        std::cout << "❌ INCORRECT RESULT (lost " 
                  << (TOTAL_EXPECTED - final_count) << " increments)" << std::endl;
    }
    
    double ops_per_second = (double)TOTAL_EXPECTED / (duration.count() / 1000000.0);
    std::cout << "Performance: " << (int)ops_per_second << " ops/second" << std::endl;
}

// =============================================================================
// Assembly inspection helper
// =============================================================================
void show_assembly_differences() {
    std::cout << "\n=== Assembly Analysis ===" << std::endl;
    std::cout << "To see assembly differences, compile with:" << std::endl;
    std::cout << "g++ -O2 -S counter_test.cpp" << std::endl;
    std::cout << "Then examine the assembly for each increment() method" << std::endl;
    
    std::cout << "\nExpected differences:" << std::endl;
    std::cout << "- Broken: Simple INC instruction (not thread-safe)" << std::endl;
    std::cout << "- Volatile: Still INC but prevents caching" << std::endl;
    std::cout << "- Mutex: CALL to lock/unlock functions" << std::endl;
    std::cout << "- Atomic: LOCK XADD or similar atomic instruction" << std::endl;
}

// =============================================================================
// Multiple run test to show race condition inconsistency
// =============================================================================
template<typename CounterType>
void race_condition_demo(const std::string& name, int runs = 5) {
    std::cout << "\n=== Race Condition Demo: " << name << " ===" << std::endl;
    std::cout << "Running " << runs << " times to show inconsistent results:" << std::endl;
    
    for (int run = 0; run < runs; ++run) {
        CounterType counter;
        std::vector<std::thread> threads;
        
        // Use fewer iterations for faster demo
        const int demo_work = 10000;
        const int expected = NUM_THREADS * demo_work;
        
        for (int i = 0; i < NUM_THREADS; ++i) {
            threads.emplace_back(worker_thread<CounterType>, 
                               std::ref(counter), demo_work);
        }
        
        for (auto& t : threads) {
            t.join();
        }
        
        int result = counter.get();
        std::cout << "Run " << (run + 1) << ": " << result 
                  << " (expected: " << expected 
                  << ", lost: " << (expected - result) << ")" << std::endl;
    }
}

// =============================================================================
// Main function
// =============================================================================
int main() {
    std::cout << "Thread-Safe Counter Exercise" << std::endl;
    std::cout << "=============================" << std::endl;
    std::cout << "Threads: " << NUM_THREADS << std::endl;
    std::cout << "Work per thread: " << WORK_PER_THREAD << std::endl;
    std::cout << "Total expected: " << TOTAL_EXPECTED << std::endl;
    std::cout << "\nCompiled with optimization? " 
              << (__OPTIMIZE__ ? "YES (-O2)" : "NO (add -O2!)") << std::endl;
    
    // Show the problems first
    race_condition_demo<BrokenCounter>("Broken Counter");
    race_condition_demo<VolatileOnlyCounter>("Volatile Only Counter");
    
    // Now benchmark all solutions
    benchmark_counter<BrokenCounter>("Broken Counter (No Sync)");
    benchmark_counter<VolatileOnlyCounter>("Volatile Only Counter");
    benchmark_counter<MutexCounter>("Mutex Counter");
    benchmark_counter<AtomicCounter>("Atomic Counter");
    benchmark_counter<AtomicRelaxedCounter>("Atomic Relaxed Counter");
    benchmark_counter<VolatileAtomicCounter>("Volatile + Atomic Counter");
    
    // Analysis
    std::cout << "\n=== Analysis ===" << std::endl;
    std::cout << "Performance ranking (fastest to slowest):" << std::endl;
    std::cout << "1. Atomic (relaxed) - Best performance, correct" << std::endl;
    std::cout << "2. Atomic (default) - Slightly slower, correct" << std::endl;
    std::cout << "3. Mutex - Much slower, correct" << std::endl;
    std::cout << "4. Broken/Volatile - Fast but WRONG results" << std::endl;
    
    std::cout << "\nKey Lessons:" << std::endl;
    std::cout << "- volatile does NOT provide atomicity" << std::endl;
    std::cout << "- volatile prevents compiler optimizations but not race conditions" << std::endl;
    std::cout << "- Atomic operations are usually faster than mutex" << std::endl;
    std::cout << "- Relaxed memory ordering can improve performance" << std::endl;
    
    show_assembly_differences();
    
    return 0;
}

/*
Compilation instructions:
========================

g++ -O2 -pthread -o counter_test counter_test.cpp

Expected results:
- Broken/Volatile: Wrong results, lost increments
- Mutex: Correct but slower
- Atomic: Correct and fast

Questions to explore:
1. Why does volatile fail to fix the race condition?
2. What's the performance difference between mutex and atomic?
3. How does memory ordering affect performance?
4. What does the assembly look like for each approach?
*/