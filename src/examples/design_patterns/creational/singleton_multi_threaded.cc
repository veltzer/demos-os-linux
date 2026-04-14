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
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <atomic>

using namespace std;

class Singleton {
public:
	static Singleton* getInstance() {
		Singleton* p = instance.load(memory_order_acquire);
		if (p == nullptr) {
			lock_guard<mutex> lock(init_mutex);
			p = instance.load(memory_order_relaxed);
			if (p == nullptr) {
				p = new Singleton();
				instance.store(p, memory_order_release);
			}
		}
		return p;
	}

	void someOperation() {
		cout << "Performing some operation in the Singleton." << endl;
	}

private:
	Singleton() {
		cout << "Singleton constructor called." << endl;
	}
	~Singleton() {
		cout << "Singleton destructor called." << endl;
	}

	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

	static atomic<Singleton*> instance;
	static mutex init_mutex;
};

atomic<Singleton*> Singleton::instance{nullptr};
mutex Singleton::init_mutex;

int main() {
	Singleton* instance1 = Singleton::getInstance();
	instance1->someOperation();
	Singleton* instance2 = Singleton::getInstance();
	instance2->someOperation();
	return EXIT_SUCCESS;
}
