#include <string>
#include <unistd.h>
#include <iostream>
#include <thread>

using namespace std;

// The function we want to execute on the new thread.
void task(string id)
{
	int counter = 0;
	while(true) {
	    cout << id << " " << counter++ << endl;
	    sleep(1);
	}
}

int main()
{
    // Constructs threads and runs them. Does not block execution.
    thread t1(task, "one");
    thread t2(task, "two");
    t1.join();
    t2.join();
}
