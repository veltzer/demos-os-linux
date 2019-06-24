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
#include <iostream>	// for std::cout, std::endl
#include <stdlib.h>	// for EXIT_SUCCESS
#include <boost/thread.hpp>	// for boost::thread::hardware_concurrency()

/*
 * A simple boost threads app
 *
 * the next one should be "-lboost_thread" but ubuntu has no symlink to that...
 * this is ugly since we have to update this every time version of boost changes
 * EXTRA_LINK_FLAGS=-l:libboost_thread.so.1.67.0 -l:libboost_system.so.1.67.0
 * XTRA_LINK_FLAGS=-lboost_thread -lboost_system
 */

int main(int argc, char** argv, char** envp) {
	size_t n_threads = boost::thread::hardware_concurrency();
	std::cout << "boost::thread::hardware_concurrency is " << n_threads << std::endl;
	return EXIT_SUCCESS;
}
