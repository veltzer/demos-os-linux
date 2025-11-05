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

#include <queue>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <mutex>

using namespace std;

template <typename T> class ThreadSafeQueue {
	queue<T> que;
	mutex mut;
	condition_variable condition;
	int waiters=0;

public:
	void push(T item) {
		lock_guard<mutex> lock(mut);
		que.push(item);
		condition.notify_one();
	}

	T pop() {
		unique_lock<mutex> lock(mutex);
		condition.wait(lock, [this]{ return !que.empty(); });
		int result = que.front();
		que.pop();
		return result;
	}
};

void producer(ThreadSafeQueue<int>& que, int id) {
	for (int i = 0; i < 5; ++i) {
		int value = id * 10 + i;
		que.push(value);
		cout << "Producer " << id << " pushed " << value << endl;
		this_thread::sleep_for(chrono::milliseconds(100));
	}
}

int main() {
}
