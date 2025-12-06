/*
 * This file is part of the demos-os-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <pthread.h>
#include <thread>
#include <iostream>
#include <chrono>

using namespace std;

class PriorityInheritanceMutex {
private:
	pthread_mutex_t mutex_;
	pthread_mutexattr_t attr_;

public:
	PriorityInheritanceMutex() {
		pthread_mutexattr_init(&attr_);
		pthread_mutexattr_setprotocol(&attr_, PTHREAD_PRIO_INHERIT);
		pthread_mutex_init(&mutex_, &attr_);
	}

	~PriorityInheritanceMutex() {
		pthread_mutex_destroy(&mutex_);
		pthread_mutexattr_destroy(&attr_);
	}

	void lock() { pthread_mutex_lock(&mutex_); }
	void unlock() { pthread_mutex_unlock(&mutex_); }
};

class PILockGuard {
	PriorityInheritanceMutex& mutex_;
public:
	explicit PILockGuard(PriorityInheritanceMutex& m) : mutex_(m) {
		mutex_.lock();
	}
	~PILockGuard() {
		mutex_.unlock();
	}
};

PriorityInheritanceMutex pi_mutex;
int shared_data = 0;

void high_priority_task() {
	cout << "High priority task waiting for mutex\n";
	PILockGuard lock(pi_mutex);
	cout << "High priority task acquired mutex\n";
	shared_data += 100;
	this_thread::sleep_for(chrono::milliseconds(10));
}

void low_priority_task() {
	cout << "Low priority task acquired mutex\n";
	PILockGuard lock(pi_mutex);
	shared_data += 1;
	this_thread::sleep_for(chrono::milliseconds(100));
	cout << "Low priority task releasing mutex\n";
}

int main() {
	thread low_thread(low_priority_task);
	this_thread::sleep_for(chrono::milliseconds(10));
	thread high_thread(high_priority_task);

	low_thread.join();
	high_thread.join();

	cout << "Final data: " << shared_data << endl;
}
