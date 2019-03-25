/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <stdlib.h>	// for EXIT_SUCCESS
#include <iostream>	// for std::cout, std::endl
#include <future>	// for std::future

/*
 * This is a basic example of C++11 async tasks.
 *
 * References:
 * http://solarianprogrammer.com/2012/10/17/cpp-11-async-tutorial
 *
 * EXTRA_COMPILE_FLAGS=-std=c++11 -pthread
 * EXTRA_LINK_FLAGS=-std=c++11 -pthread -Wl,--no-as-needed
 */

int main(int argc, char** argv, char** envp) {
	// called_from_async launched in a separate thread if possible
	std::future<int> result(std::async([](int m, int n) { return m + n;} , 2, 4));
	std::cout << "Message from main" << std::endl;
	//retrive and print the value stored in the future
	std::cout << result.get() << std::endl;
	return EXIT_SUCCESS;
}
