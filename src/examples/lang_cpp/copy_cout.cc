/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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

/*
This example shows how to create an ostream object around standard output
*/

#include <firstinclude.h>
#include <stdlib.h> // for EXIT_SUCCESS
#include <iostream> // for std::cout, std::endl
#include <ext/stdio_filebuf.h> // for _gnu_cxx::stdio_filebuf<T>
#include <unistd.h> // for STDOUT_FILENO

int main(int argc, char** argv, char** envp) {
	std::cout << "Hello, World!" << std::endl;
	std::cout.flush();
	__gnu_cxx::stdio_filebuf<char> filebuf(STDOUT_FILENO, std::ios::out);
	std::ostream* os=new std::ostream(&filebuf);
	*os << "And hello from me too" << std::endl;
	os->flush();
	delete os;
	std::cout << "Hello, World! (2nd time)" << std::endl;
	return EXIT_SUCCESS;
}
