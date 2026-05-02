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

/*
 * Demonstrates the classic singleton race condition.
 * The unsynchronized check-then-act in getInstance() lets multiple
 * threads each create their own instance. Run repeatedly to observe
 * the non-determinism.
 */

class Singleton {
private:
	static Singleton* instance;
	int value;

	Singleton() {
		this_thread::sleep_for(chrono::milliseconds(10));
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<> dis(1, 1000);
		value = dis(gen);
		cout << "Created instance at address: " << this << " with value: " << value << endl;
	}

public:
	static Singleton* getInstance() {
		if (instance == nullptr) {
			this_thread::sleep_for(chrono::milliseconds(5));
			instance = new Singleton();
		}
		return instance;
	}

	int getValue() const {
		return value;
	}

	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

	void* getAddress() const {
		return (void*)this;
	}

	static void reset() {
		delete instance;
		instance = nullptr;
	}
};

Singleton* Singleton::instance = nullptr;

void createSingleton(int threadId, vector<pair<int, void*>>& results, mutex& resultsMutex) {
	cout << "Thread " << threadId << " starting..." << endl;
	Singleton* singleton = Singleton::getInstance();
	cout << "Thread " << threadId << " got instance at: " << singleton->getAddress()
		<< " with value: " << singleton->getValue() << endl;
	lock_guard<mutex> lock(resultsMutex);
	results.push_back({threadId, singleton->getAddress()});
}

int main() {
	cout << "=== Demonstrating C++ Singleton Race Condition ===\n" << endl;
	const int NUM_THREADS = 5;
	vector<thread> threads;
	vector<pair<int, void*>> results;
	mutex resultsMutex;
	for(int i = 0; i < NUM_THREADS; ++i) {
		threads.emplace_back(createSingleton, i, ref(results), ref(resultsMutex));
	}
	for(auto& thread : threads) {
		thread.join();
	}
	cout << "\n=== Results ===" << endl;
	set<void*> uniqueInstances;
	for(const auto& result : results) {
		cout << "Thread " << result.first << ": Instance at " << result.second << endl;
		uniqueInstances.insert(result.second);
	}
	cout << "\nTotal unique instances created: " << uniqueInstances.size() << endl;
	if (uniqueInstances.size() > 1) {
		cout << "RACE CONDITION DETECTED! Multiple instances were created." << endl;
	} else {
		cout << "No race condition occurred this time (try running again)." << endl;
	}
	cout << "\n" << string(50, '=') << endl;
	cout << "Run this program multiple times to see the race condition!" << endl;
	cout << "The number of instances created may vary between runs." << endl;
	return 0;
}
