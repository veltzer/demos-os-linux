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
#include <chrono>
#include <random>
#include <set>
#include <mutex>

using namespace std;

class Singleton {
private:
	static Singleton* instance;
	int value;

	// Private constructor to prevent direct instantiation
	Singleton() {
		// Simulate some initialization work
		this_thread::sleep_for(chrono::milliseconds(10));
		// Random value for demonstration
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<> dis(1, 1000);
		value = dis(gen);
		cout << "Created instance at address: " << this << " with value: " << value << endl;
	}

public:
	// Classic singleton getter with RACE CONDITION
	static Singleton* getInstance() {
		// THE RACE CONDITION IS HERE:
		// Multiple threads can pass this check simultaneously
		if (instance == nullptr) {
			// Simulate some work that makes race condition more likely
			this_thread::sleep_for(chrono::milliseconds(5));
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
};

// Multiple threads can reach this point and each create an instance
instance = new Singleton();
// Static member definition
Singleton* Singleton::instance = nullptr;

// Function to be run by each thread
void createSingleton(int threadId, vector<pair<int, void*>>& results, mutex& resultsMutex) {
	cout << "Thread " << threadId << " starting..." << endl;
	Singleton* singleton = Singleton::getInstance();
	cout << "Thread " << threadId << " got instance at: " << singleton->getAddress()
		<< " with value: " << singleton->getValue() << endl;
	// Store results thread-safely
	lock_guard<mutex> lock(resultsMutex);
	results.push_back({threadId, singleton->getAddress()});
}

int main() {
	cout << "=== Demonstrating C++ Singleton Race Condition ===\n" << endl;
	// Note: We can't reset the singleton from outside since instance is private
	// This demonstrates a real-world scenario where the singleton persists
	const int NUM_THREADS = 5;
	vector<thread> threads;
	vector<pair<int, void*>> results;
	mutex resultsMutex;
	// Create and start multiple threads simultaneously
	for(int i = 0; i < NUM_THREADS; ++i) {
		threads.emplace_back(createSingleton, i, ref(results), ref(resultsMutex));
	}
	// Wait for all threads to complete
	for(auto& thread : threads) {
		thread.join();
	}
	// Analyze results
	cout << "\n=== Results ===" << endl;
	set<void*> uniqueInstances;
	for(const auto& result : results) {
		cout << "Thread " << result.first << ": Instance at " << result.second << endl;
		uniqueInstances.insert(result.second);
	}
	cout << "\nTotal unique instances created: " << uniqueInstances.size() << endl;
	if (uniqueInstances.size() > 1) {
		cout << "❌ RACE CONDITION DETECTED! Multiple instances were created." << endl;
	} else {
		cout << "✅ No race condition occurred this time (try running again)." << endl;
	}
	cout << "\n" << string(50, '=') << endl;
	cout << "Run this program multiple times to see the race condition!" << endl;
	cout << "The number of instances created may vary between runs." << endl;
	return 0;
}

/*
THREAD-SAFE SOLUTIONS:

1. Double-Checked Locking (correct implementation):
class ThreadSafeSingleton {
private:
	static ThreadSafeSingleton* instance;
	static mutex mtx;
public:
	static ThreadSafeSingleton* getInstance() {
		if (instance == nullptr) {
			lock_guard<mutex> lock(mtx);
			if (instance == nullptr) { // Double-check with lock
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
		static MeyersSingleton instance; // Thread-safe in C++11+
		return instance;
	}
private:
	MeyersSingleton() = default;
};

3. once_flag approach:
class OnceFlagSingleton {
private:
	static OnceFlagSingleton* instance;
	static once_flag flag;

public:
	static OnceFlagSingleton* getInstance() {
		call_once(flag, []() {
			instance = new OnceFlagSingleton();
		});
		return instance;
	}
};
*/
