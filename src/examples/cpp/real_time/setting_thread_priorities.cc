#include <sched.h>
#include <thread>
#include <iostream>

void set_thread_priority(std::thread& t, int priority) {
    sched_param param;
    param.sched_priority = priority;
    pthread_setschedparam(t.native_handle(), SCHED_FIFO, &param);
}

void high_priority_task() {
    std::cout << "High priority task waiting for mutex\n";
    std::cout << "High priority task acquired mutex\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void low_priority_task() {
    std::cout << "Low priority task acquired mutex\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Low priority task releasing mutex\n";
}

int main() {
    std::thread low_thread(low_priority_task);
    std::thread high_thread(high_priority_task);
    
    // Set priorities (requires root or CAP_SYS_NICE)
    set_thread_priority(low_thread, 10);   // Low priority
    set_thread_priority(high_thread, 90);  // High priority
    
    low_thread.join();
    high_thread.join();
}
