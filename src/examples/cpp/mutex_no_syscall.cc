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
#include <cstdlib>
#include <mutex>

using namespace std;

/*
 * This example shows that mutexes in linux, even in C++ are really futexes which means
 * that they do not issue any syscalls to the OS.
 *
 * Compare the output of ltrace and strace to this program to see the difference.
 */

int main() {
	mutex m;
	for(int i=0; i<1000000; i++) {
		m.lock();
		m.unlock();
	}
	return EXIT_SUCCESS;
}
