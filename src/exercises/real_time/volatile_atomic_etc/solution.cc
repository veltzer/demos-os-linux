/*
 * This file is part of the demos-os-linux package.
 * Copyright (C) 2011-2026 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <chrono>
#include <cassert>

/*
 * EXCLUDE_PROFILE=clang
 */

using namespace std;

// Configuration
constexpr int NUM_THREADS = 4;
constexpr int WORK_PER_THREAD = 100000;
constexpr int TOTAL_EXPECTED = NUM_THREADS * WORK_PER_THREAD;

// =============================================================================
// Solution 1: BROKEN - No synchronization (shows the problem)
// =============================================================================
class BrokenCounter {
private:
	int counter; // NO volatile, NO synchronization

public:
	BrokenCounter() : counter(0) {}

	void increment() {
		counter++; // Race condition!
	}

	int get() const { return counter; }
	const char* name() const { return "Broken (no sync)"; }
};

// =============================================================================
// Solution 2: STILL BROKEN - Volatile only (shows volatile isn't enough)
// =============================================================================
class VolatileOnlyCounter {
private:
	volatile int counter; // volatile but no atomicity

public:
	VolatileOnlyCounter() : counter(0) {}

	void increment() {
		counter++; // Still a race condition! Read-modify-write not atomic
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
	mutable mutex mtx;

public:
	MutexCounter() : counter(0) {}

	void increment() {
		lock_guard<mutex> lock(mtx);
		counter++;
	}

	int get() const {
		lock_guard<mutex> lock(mtx);
		return counter;
	}

	const char* name() const { return "Mutex protected"; }
};

// =============================================================================
// Solution 4: CORRECT - Atomic operations
// =============================================================================
class AtomicCounter {
private:
	atomic<int> counter;

public:
	AtomicCounter() : counter(0) {}

	void increment() {
		counter++; // Atomic increment
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
	atomic<int> counter;

public:
	AtomicRelaxedCounter() : counter(0) {}

	void increment() {
		counter.fetch_add(1, memory_order_relaxed);
	}

	int get() const {
		return counter.load(memory_order_relaxed);
	}

	const char* name() const { return "Atomic (relaxed ordering)"; }
};

// =============================================================================
// Solution 6: HYBRID - Volatile + Atomic operations (educational)
// =============================================================================
class VolatileAtomicCounter {
private:
	volatile atomic<int> counter; // Both volatile AND atomic

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
	for(int i = 0; i < work_count; ++i) {
		// Simulate some work
		volatile int dummy = 0;
		for(int j = 0; j < 100; ++j) {
			dummy += j; // Prevent optimization
		}

		// Increment counter after "work" is done
		counter.increment();
	}
}

// =============================================================================
// Benchmark function template
// =============================================================================
template<typename CounterType>
void benchmark_counter(const string& test_name) {
	cout << "\n=== " << test_name << " ===" << endl;

	CounterType counter;
	vector<thread> threads;

	// Start timing
	auto start = chrono::high_resolution_clock::now();

	// Create and start threads
	for(int i = 0; i < NUM_THREADS; ++i) {
		threads.emplace_back(worker_thread<CounterType>,
						ref(counter), WORK_PER_THREAD);
	}

	// Wait for all threads to complete
	for(auto& t : threads) {
		t.join();
	}

	auto end = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

	// Results
	int final_count = counter.get();
	cout << "Counter type: " << counter.name() << endl;
	cout << "Final count: " << final_count
			<< " (expected: " << TOTAL_EXPECTED << ")" << endl;
	cout << "Time taken: " << duration.count() << " microseconds" << endl;

	if (final_count == TOTAL_EXPECTED) {
		cout << "✅ CORRECT RESULT" << endl;
	} else {
		cout << "❌ INCORRECT RESULT (lost "
				<< (TOTAL_EXPECTED - final_count) << " increments)" << endl;
	}

	double ops_per_second = (double)TOTAL_EXPECTED / (duration.count() / 1000000.0);
	cout << "Performance: " << (int)ops_per_second << " ops/second" << endl;
}

// =============================================================================
// Assembly inspection helper
// =============================================================================
void show_assembly_differences() {
	cout << "\n=== Assembly Analysis ===" << endl;
	cout << "To see assembly differences, compile with:" << endl;
	cout << "g++ -O2 -S counter_test.cpp" << endl;
	cout << "Then examine the assembly for each increment() method" << endl;

	cout << "\nExpected differences:" << endl;
	cout << "- Broken: Simple INC instruction (not thread-safe)" << endl;
	cout << "- Volatile: Still INC but prevents caching" << endl;
	cout << "- Mutex: CALL to lock/unlock functions" << endl;
	cout << "- Atomic: LOCK XADD or similar atomic instruction" << endl;
}

// =============================================================================
// Multiple run test to show race condition inconsistency
// =============================================================================
template<typename CounterType>
void race_condition_demo(const string& name, int runs = 5) {
	cout << "\n=== Race Condition Demo: " << name << " ===" << endl;
	cout << "Running " << runs << " times to show inconsistent results:" << endl;

	for(int run = 0; run < runs; ++run) {
		CounterType counter;
		vector<thread> threads;

		// Use fewer iterations for faster demo
		const int demo_work = 10000;
		const int expected = NUM_THREADS * demo_work;

		for(int i = 0; i < NUM_THREADS; ++i) {
			threads.emplace_back(worker_thread<CounterType>,
							ref(counter), demo_work);
		}

		for(auto& t : threads) {
			t.join();
		}

		int result = counter.get();
		cout << "Run " << (run + 1) << ": " << result
				<< " (expected: " << expected
				<< ", lost: " << (expected - result) << ")" << endl;
	}
}

// =============================================================================
// Main function
// =============================================================================
int main() {
	cout << "Thread-Safe Counter Exercise" << endl;
	cout << "=============================" << endl;
	cout << "Threads: " << NUM_THREADS << endl;
	cout << "Work per thread: " << WORK_PER_THREAD << endl;
	cout << "Total expected: " << TOTAL_EXPECTED << endl;
	cout << "\nCompiled with optimization? "
			<< (__OPTIMIZE__ ? "YES (-O2)" : "NO (add -O2!)") << endl;

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
	cout << "\n=== Analysis ===" << endl;
	cout << "Performance ranking (fastest to slowest):" << endl;
	cout << "1. Atomic (relaxed) - Best performance, correct" << endl;
	cout << "2. Atomic (default) - Slightly slower, correct" << endl;
	cout << "3. Mutex - Much slower, correct" << endl;
	cout << "4. Broken/Volatile - Fast but WRONG results" << endl;

	cout << "\nKey Lessons:" << endl;
	cout << "- volatile does NOT provide atomicity" << endl;
	cout << "- volatile prevents compiler optimizations but not race conditions" << endl;
	cout << "- Atomic operations are usually faster than mutex" << endl;
	cout << "- Relaxed memory ordering can improve performance" << endl;

	show_assembly_differences();

	return EXIT_SUCCESS;
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
