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

template <typename T> class ThreadSafeQueue {
	std::queue<T > queue;
	std::mutex mutex;
	std::condition_variable condition;
	int waiters=0;

public:
	void push(T item) {
		std::lock_guard<std::mutex> lock(mutex);
		queue.push(item);
		condition.notify_one();
	}

	T pop() {
		std::unique_lock<std::mutex> lock(mutex);
		condition.wait(lock, [this]{ return !queue.empty(); });
		int result = queue.front();
		queue.pop();
		return result;
	}
};

void producer(ThreadSafeQueue<int>& queue, int id) {
	for (int i = 0; i < 5; ++i) {
		int value = id * 10 + i;
		queue.push(value);
		std::cout << "Producer " << id << " pushed " << value << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

int main() {
}
