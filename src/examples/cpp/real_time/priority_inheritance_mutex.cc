#include <pthread.h>
#include <thread>
#include <iostream>
#include <chrono>

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
    std::cout << "High priority task waiting for mutex\n";
    PILockGuard lock(pi_mutex);
    std::cout << "High priority task acquired mutex\n";
    shared_data += 100;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void low_priority_task() {
    std::cout << "Low priority task acquired mutex\n";
    PILockGuard lock(pi_mutex);
    shared_data += 1;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Low priority task releasing mutex\n";
}

int main() {
    std::thread low_thread(low_priority_task);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::thread high_thread(high_priority_task);
    
    low_thread.join();
    high_thread.join();
    
    std::cout << "Final data: " << shared_data << std::endl;
}
