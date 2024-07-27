/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2024 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <cstdlib>
#include <iostream>

using namespace std;

/*
 * This example shows how to use the C++ operator new placement
 * operator.
 *
 * Things we learn:
 * 1. How to write your own placement function.
 * 2. Regular constructor gets called after the placement.
 * 3. Releasing of space could be overridden too.
 * 4. This could be used for caching and real time considerations for instance.
 * 5. Even if you allocate an array the delete[] is NOT called so
 * your regular delete operator needs to know how to do the job
 * both for arrays and for single elements (if you want arrays
 * at all that is...).
 *
 * TODO:
 * - show in place construction (calling the constructor on an otherwise
 * allocated block of ram)
 */

class A{
public:
	float val;
	A(void) {
		val=-7.6;
	}

	A(double ival) {
		val=ival;
	}

	void* operator new(size_t size, double) {
		cout << "in new operator" << endl;
		cout << "size is " << size << endl;
		void* pointer=malloc(size);
		cout << "pointer is " << pointer << endl;
		// next two lines have no effect since the constructor
		// will be called and will override it
		// A *p=(A *)pointer;
		// p->val=val;
		return pointer;
	}

	// this is for allocating arrays, the size that you get
	// is SizeOfObject*NumOfObjects...
	void* operator new[] (const size_t size) {
		cout << "in new[] operator" << endl;
		cout << "size is " << size << endl;
		void* pointer=malloc(size);
		cout << "pointer is " << pointer << endl;
		return pointer;
	}

	// notice that this does NOT get called...
	void operator delete[] (void* pointer) {
		cout << "in delete[] operator" << endl;
		cout << "pointer is " << pointer << endl;
		free(pointer);
	}

	void* operator new(size_t size) {
		cout << "in new operator" << endl;
		cout << "size is " << size << endl;
		// void* pointer=new char[size];
		void* pointer=malloc(size);
		cout << "pointer is " << pointer << endl;
		return pointer;
	}

	void operator delete(void* pointer) {
		cout << "in delete operator" << endl;
		cout << "pointer is " << pointer << endl;
		free(pointer);
	}
};

int main() {
	cout << "heap no arguments example" << endl;
	A* a=new A();

	cout << "a->val is " << a->val << endl;
#pragma GCC diagnostic ignored "-Wmismatched-new-delete"
	delete a;

	cout << "heap arguments example" << endl;
	A* b=new(5.5)A();
	cout << "b->val is " << b->val << endl;
#pragma GCC diagnostic ignored "-Wmismatched-new-delete"
	delete b;

	cout << "many heap no arguments example" << endl;
	const unsigned int num_objs=5;
	A* e=new A[num_objs];
	for(unsigned int i=0; i<num_objs; i++) {
		cout << i << " " << "e->val is " << e[i].val << endl;
	}
	delete[] e;

	// the next two examples are stack examples in which case neither
	// the new nor the delete operator will be called (memory is stack
	// memory).
	// could you write a C++ object which can be used ONLY on the stack
	// or conversly ONLY on the heap using this property ?!?

	cout << "stack no arguments example" << endl;
	A c;
	cout << "c.val is " << c.val << endl;

	cout << "stack arguments example" << endl;
	A d(6.7);

	cout << "d.val is " << d.val << endl;
	return EXIT_SUCCESS;
}
