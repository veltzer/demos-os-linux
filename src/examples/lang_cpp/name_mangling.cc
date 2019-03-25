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

/*
 * This exampe shows how to demangle names on the command line using
 * nm
 */

#include <firstinclude.h>
#include <iostream>	// for std::cout, std::endl
#include <multiproc_utils.h>	// for my_system()

class A {
public:
	void doit();
};

void A::doit() {
	std::cout << "Hello, World!" << std::endl;
}

int main(int argc, char** argv, char** envp) {
	A a;
	a.doit();
	my_system("nm -C %s | grep A::", argv[0]);
	return 0;
}
