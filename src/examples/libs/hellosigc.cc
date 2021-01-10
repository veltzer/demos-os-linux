/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <iostream>	// for std::cout
#include <string>	// for std::string
#include <sigc++/sigc++.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This is a demo program for using the sigc++ signaling library
 *
 * EXTRA_COMPILE_CMDS=pkg-config --cflags sigc++-2.0
 * EXTRA_LINK_CMDS=pkg-config --libs sigc++-2.0
 */

void on_print(const std::string& str) {
	std::cout << str;
}

int main(int argc, char** argv, char** envp) {
	sigc::signal<void, const std::string&> signal_print;
	signal_print.connect(sigc::ptr_fun(&on_print));
	signal_print.emit("hello world\n");
	return EXIT_SUCCESS;
}
