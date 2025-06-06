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
 * - many other weird side effects happen. Play around with the numbers and
 * you will see it.
 *
 * Note that all of these behaviours are particular to the GNU implementation
 * of STL and are not part of the STL standard. A different STL implementation
 * may yield totally different outcomes.
 */

static int listorvector, array_size, postoact, postoremove, numtoremove;

void do_list() {
	list<int> l;
	for(int i=0; i<array_size; i++) {
		l.push_back(i);
	}
	list<int>::iterator i;
	int counter=0;
	for(i=l.begin(); i!=l.end(); i++) {
		if(counter==postoact) {
			for(int j=0; j<numtoremove; j++) {
				l.remove(postoremove+j);
			}
		}
		cout << "visiting " << *i << endl;
		counter++;
	}
}

void do_vector() {
	vector<int> l;
	for(int i=0; i<array_size; i++) {
		l.push_back(i);
	}
	vector<int>::iterator i;
	int counter=0;
	for(i=l.begin(); i!=l.end(); i++) {
		if(counter==postoact) {
			for(int j=0; j<numtoremove; j++) {
				l.erase(l.begin()+postoremove);
			}
		}
		// cppcheck-suppress invalidContainer
		cout << "visiting " << *i << endl;
		counter++;
	}
}

int main(int argc, char** argv) {
	if(argc!=6) {
		cerr << argv[0] << ": usage: " << argv[0] << " [list(1) or vector(0)] [array_size] [postoact] [postoremove] [numtoremove]" << endl;
		cerr << argv[0] << ": example: " << argv[0] << " 1 10 5 5 1" << endl;
		return EXIT_FAILURE;
	}
	listorvector=atoi(argv[1]);
	array_size=atoi(argv[2]);
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
