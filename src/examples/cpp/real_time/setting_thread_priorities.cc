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
#include <sched.h>
#include <thread>
#include <iostream>

using namespace std;

void set_thread_priority(thread& t, int priority) {
	sched_param param;
	param.sched_priority = priority;
	pthread_setschedparam(t.native_handle(), SCHED_FIFO, &param);
}

void high_priority_task() {
	cout << "High priority task waiting for mutex\n";
	cout << "High priority task acquired mutex\n";
	this_thread::sleep_for(chrono::milliseconds(10));
}

void low_priority_task() {
	cout << "Low priority task acquired mutex\n";
	this_thread::sleep_for(chrono::milliseconds(100));
	cout << "Low priority task releasing mutex\n";
}

int main() {
	thread low_thread(low_priority_task);
	thread high_thread(high_priority_task);

	// Set priorities (requires root or CAP_SYS_NICE)
	set_thread_priority(low_thread, 10); // Low priority
	set_thread_priority(high_thread, 90); // High priority

	low_thread.join();
	high_thread.join();
}
