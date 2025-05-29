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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This is an example of an object deallocating itself.
 * The idea is to put a "delete this" at the VERY END of some
 * method.
 * You cannot put this in the destructor since you will cause
 * an infinite regress.
 *
 * What are the problems with this approach ?!?
 * Well - "delete this" requires that the object in question was
 * allocated on the heap and not say, stack or data segment. In
 * that cause doing "delete this" may cause a segfault or maybe
 * even a silent bug which is worse at times.
 */

class A{
// private:
// int secret;

public:
	A(void) {
		printf("This is the constructor\n");
	}
	~A(void) {
		printf("This is the destructor\n");
	}
	void doit(void) {
		// the next line must be the very LAST line
		// in this methods code (after any business
		// logic).
		delete this;
	}
};

// here is an object that causes infinite recursion in it's destructor...
class B{
public: ~B(void) {
		delete this;	// NOLINT
	}
};

int main() {
	A* a=new A();
	a->doit();
	// this is a problem since the next allocation is on the stack and not
	// on the heap...
	// A aa;
	// aa.doit();
	// the next code will cause infinite recursion and error...
	// B* b=new B();
	// delete b;
	return EXIT_SUCCESS;
}
