/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2024 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <string>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>


using namespace std;


// A function to be executed in newly created threads
void task(const string id) {
	int counter = 0;
	while(true) {
		cout << id << " " << counter++ << endl;
		this_thread::sleep_for(chrono::seconds(1));
	}
}

int main() {
	// Constructs threads and runs them. Does not block execution.
	thread t1(task, "one");
	thread t2(task, "two");
	t1.join();
	t2.join();
	return EXIT_SUCCESS;
}
