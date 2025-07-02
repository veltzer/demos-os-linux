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
