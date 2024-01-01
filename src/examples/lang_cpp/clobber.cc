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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This is a demo that shows C++ allows to clobber variable names in serveral ways:
 * - inheritance.
 * - local parameters.
 */

class A {
protected:
	int a;
	int b;
};

class B : public A {
public:
	int c;
	int b;

	void doSomething(int a) {
		a=5;
		b=6;
		c=8;
		printf("a is %d\n", a);
		printf("b is %d\n", b);
		printf("c is %d\n", c);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
		printf("this->a is %d\n", this->a);
#pragma GCC diagnostic pop
		printf("this->b is %d\n", this->b);
		printf("this->c is %d\n", this->c);
	}
};

int main(int argc, char** argv, char** envp) {
	B b;
	b.doSomething(7);
	return EXIT_SUCCESS;
}
