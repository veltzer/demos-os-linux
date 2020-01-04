/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
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

#include "connect.h"

/* functions that we need to supply */
extern "C" {
/* support functions */
void myprintk(const char *);
void *mymalloc(unsigned int);
void myfree(void *);
}

/* to satisfy the kernel dynamic linker */
int __gxx_personality_v0;
int _Unwind_Resume;

#ifdef __i386__
typedef unsigned int size_t;
#else
typedef unsigned long size_t;
#endif	// __i386__

/* support code for new and delete */
void *operator new(size_t x) {
	return(mymalloc(x));
}

void operator delete(void *pointer) {
	myfree(pointer);
}

class A {
public:
	int x;
	A() {
		myprintk("inside A's constructor!\n");
	}
	~A() {
		myprintk("inside A's destructor!\n");
	}
};

static A *a;

int cpp_init() {
	a=new A();

	return(0);
}

void cpp_exit() {
	delete a;
}
