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
#include <list>	// for std::list<T>, std::list<T>::iterator
#include <vector>	// for std::list<T>, std::list<T>::iterator
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE;

/*
 * This example explores the performance of vector with regard
 * to memcpy...
 *
 * EXTRA_COMPILE_FLAGS=-g3
 */

static int loop, size, modulu;

void abuse_vector_once() {
	std::vector<int> v;
	for(int i=0; i<size; i++) {
		v.push_back(i);
		if(i%modulu==0) {
			void* p __attribute__((unused))=malloc(2);
		}
	}
}

int main(int argc, char** argv, char** envp) {
	if(argc!=4) {
		std::cerr << argv[0] << ": usage: " << argv[0] << " [loop] [size] [modulu]" << std::endl;
		return EXIT_FAILURE;
	}
	loop=atoi(argv[1]);
	size=atoi(argv[2]);
	modulu=atoi(argv[3]);
	for(int i=0; i<loop; i++) {
		abuse_vector_once();
	}
	return EXIT_SUCCESS;
}
