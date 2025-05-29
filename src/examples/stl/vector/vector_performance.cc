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
#include <iostream>	// for cout, endl
#include <list>	// for list<T>, list<T>::iterator
#include <vector>	// for list<T>, list<T>::iterator
#include <cstdlib>	// for EXIT_SUCCESS, EXIT_FAILURE;

using namespace std;

/*
 * This example explores the performance of vector with regard
 * to memcpy...
 *
 * EXTRA_COMPILE_FLAGS=-g3
 */

static int loop, array_size, modulu;

void abuse_vector_once() {
	vector<int> v;
	for(int i=0; i<array_size; i++) {
		v.push_back(i);
		if(i%modulu==0) {
			void* p __attribute__((unused))=malloc(2);
		}
	}
}

int main(int argc, char** argv) {
	if(argc!=4) {
		cerr << argv[0] << ": usage: " << argv[0] << " [loop] [array_size] [modulu]" << endl;
		return EXIT_FAILURE;
	}
	loop=atoi(argv[1]);
	array_size=atoi(argv[2]);
	modulu=atoi(argv[3]);
	for(int i=0; i<loop; i++) {
		abuse_vector_once();
	}
	return EXIT_SUCCESS;
}
