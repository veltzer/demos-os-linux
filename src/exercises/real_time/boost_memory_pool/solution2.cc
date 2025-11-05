/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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

#include <firstinclude.h>
#include <boost/pool/pool.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <stdexcept>

using namespace std;

class Person {
private:
	string name_;
	int age_;

public:
	Person() : name_(""), age_(0) {}

	const string& getName() const { return name_; }
	int getAge() const { return age_; }

	void setName(const string& name) { name_ = name; }
	void setAge(int age) { age_ = age; }

	void reset() {
		name_.clear();
		age_ = 0;
	}

	void display() const {
		cout << "Person: " << name_ << ", Age: " << age_ << endl;
	}
};

class PersonPool {
private:
	boost::pool<> memory_pool_;
	vector<Person*> available_;
	mutex mutex_;

public:
	PersonPool(size_t count) : memory_pool_(sizeof(Person)) {
		// Pre-allocate all objects
		for(size_t i = 0; i < count; ++i) {
			void* mem = memory_pool_.malloc();
			Person* person = new(mem) Person(); // Placement new
			available_.push_back(person);
		}
	}

	~PersonPool() {
		lock_guard<mutex> lock(mutex_);
		// Call destructors for all objects
		for(Person* p : available_) {
			p->~Person();
		}
	}

	Person* get() {
		lock_guard<mutex> lock(mutex_);
		if (available_.empty()) {
			throw runtime_error("Pool exhausted");
		}

		Person* person = available_.back();
		available_.pop_back();
		return person;
	}

	void put(Person* person) {
		if (!person) return;

		lock_guard<mutex> lock(mutex_);
		person->reset();
		available_.push_back(person);
	}

	size_t available_count() const {
		lock_guard<mutex> lock(const_cast<mutex&>(mutex_));
		return available_.size();
	}
};

int main() {
	// Create pool with exactly 100 pre-allocated Person objects
	PersonPool pool(100);

	cout << "Pool created with " << pool.available_count() << " objects\n";

	// Get objects from pool
	Person* p1 = pool.get();
	Person* p2 = pool.get();

	cout << "After getting 2 objects: " << pool.available_count() << " available\n";

	// Use the objects
	p1->setName("Alice");
	p1->setAge(25);

	p2->setName("Bob");
	p2->setAge(30);

	p1->display();
	p2->display();

	// Return objects to pool
	pool.put(p1);
	pool.put(p2);

	cout << "After returning objects: " << pool.available_count() << " available\n";

	// Test pool exhaustion
	vector<Person*> persons;
	try {
		// Get all 100 objects
		for(int i = 0; i < 100; ++i) {
			persons.push_back(pool.get());
		}
		cout << "Successfully got all 100 objects\n";

		// Try to get one more (should throw)
		Person* extra = pool.get();
		cout << "ERROR: Should not reach here!\n";
	extra->display();

	} catch (const runtime_error& e) {
		cout << "Expected exception: " << e.what() << endl;
	}

	// Return all objects
	for(Person* p : persons) {
		pool.put(p);
	}

	cout << "Final count: " << pool.available_count() << " available\n";

	return EXIT_SUCCESS;
}
