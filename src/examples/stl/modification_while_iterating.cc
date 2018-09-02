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
 * This example shows that doing modification on an STL data structure while
 * iterating it could produce quite interesting results which are hard to
 * predict or understand. It does not only depend on what it is that you
 * are trying to do but also on what data structure you are actually using:
 * list or vector. In this example we explore both of these and show that
 * there are differences in behaviour between them:
 * - vector dumps core when trying to remove elements which are not in it
 * using its 'erase' method.
 * - list forgives trying to remove elements which are not there using its
 * 'remove' method.
 * - vector is ok with removing the current element.
 * - list dumps core when removing the current element.
 * - many other weird side effects happend. Play around with the numbers and
 * you will see it.
 *
 * Note that all of these behaviours are particular to the GNU implementation
 * of STL and are not part of the STL standard. A different STL implementation
 * may yield totally different outcomes.
 */

static int listorvector, size, postoact, postoremove, numtoremove;

void do_list() {
	std::list<int> l;
	for(int i=0; i<size; i++) {
		l.push_back(i);
	}
	// remove an element in a position already passed
	std::list<int>::iterator i;
	int counter=0;
	for(i=l.begin(); i!=l.end(); i++) {
		if(counter==postoact) {
			for(int j=0; j<numtoremove; j++) {
				l.remove(postoremove+j);
			}
		}
		std::cout << "visiting " << *i << std::endl;
		counter++;
	}
}

void do_vector() {
	std::vector<int> l;
	for(int i=0; i<size; i++) {
		l.push_back(i);
	}
	// remove an element in a position already passed
	std::vector<int>::iterator i;
	int counter=0;
	for(i=l.begin(); i!=l.end(); i++) {
		if(counter==postoact) {
			for(int j=0; j<numtoremove; j++) {
				l.erase(l.begin()+postoremove);
			}
		}
		std::cout << "visiting " << *i << std::endl;
		counter++;
	}
}

int main(int argc, char** argv, char** envp) {
	if(argc!=6) {
		std::cerr << argv[0] << ": usage: " << argv[0] << " [list or vector (1 for list)] [size] [postoact] [postoremove] [numtoremove]" << std::endl;
		std::cerr << argv[0] << ": example: " << argv[0] << " 1 10 5 5 1" << std::endl;
		return EXIT_FAILURE;
	}
	listorvector=atoi(argv[1]);
	size=atoi(argv[2]);
	postoact=atoi(argv[3]);
	postoremove=atoi(argv[4]);
	numtoremove=atoi(argv[5]);
	if(listorvector) {
		do_list();
	} else {
		do_vector();
	}
	return EXIT_SUCCESS;
}
