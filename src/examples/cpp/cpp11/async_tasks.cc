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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <iostream>	// for cout, endl
#include <thread>	// for this_thread
#include <vector>	// for vector
#include <future>	// for future

using namespace std;

/*
 * This is an example of C++11 async tasks.
 * In this example they are all executed by the one "real" thread.
 * But you could cause many threads to execute these async tasks
 * if you add a thread pool.
 *
 * References:
 * http://bartoszmilewski.com/2011/10/10/async-tasks-in-c11-not-quite-there-yet
 *
 * TODO:
 * - add a thread pool to show that we could use many threads to execute the tasks
 * in parallel.
 */

int main() {
	cout << "Main thread id: " << this_thread::get_id() << endl;
	vector<future<void> > futures;
	const unsigned int task_num=20;
	for(unsigned int i = 0; i < task_num; ++i) {
		futures.push_back(async([i] {
			this_thread::sleep_for(chrono::seconds(1));
			cout << i << ": " << this_thread::get_id() << " " << endl;
		}));
	}
	for(auto& fut:futures) {
		fut.get();
	}
}
