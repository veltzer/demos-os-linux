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
#include <iostream>	// for cout
#include <string>	// for string
#include <sigc++/sigc++.h>
#include <cstdlib>

using namespace std;

/*
 * This is a demo program for using the sigc++ signaling library
 *
 * EXTRA_COMPILE_CMD=pkg-config --cflags sigc++-2.0
 * EXTRA_LINK_CMD=pkg-config --libs sigc++-2.0
 */

void on_print(const string& str) {
	cout << str;
}

int main() {
	sigc::signal<void, const string&> signal_print;
	signal_print.connect(sigc::ptr_fun(&on_print));
	signal_print.emit("hello world\n");
	return EXIT_SUCCESS;
}
