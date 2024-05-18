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
 * This example shows that private in C++ is protection only for compile time
 * and not for run time.
 *
 * TODO:
 * - use run time type information in order to determine at runtime if a class has
 *	a virtual table or not and so simplify the 'setSecret' function even more.
 */

/*
 * This is supposed to be a class which does not allow access to it's secret private,
 * only through accessors...
 */
class A{
private:
	int secret;

public:
	A(void) {
		secret=0;
	}
	int getSecret(void) {
		return secret;
	}
	void setSecret(int val) {
		secret=val;
	}
};
/*
 * A similar class but with virtual methods...
 */
class B{
private:
	int secret;

public:
	B(void) {
		secret=0;
	}
	int getSecret(void) {
		return secret;
	}
	void setSecret(int val) {
		secret=val;
	}
	virtual void doit(void) {
		printf("in doit\n");
	}
};

/*
 * This is the function that does the fiddling. It touches the object of your choice.
 * In this version you have to tell it whether or not the object passed to it has virtual
 * function or not.
 */
void setSecret(void* a, int val, bool has_virt) {
	int* p=(int*)a;
	if(has_virt) {
		p++;
	}
	*p=val;
}

int main() {
	A a;
	B b;
	printf("a.getSecret() is %d\n", a.getSecret());
	printf("b.getSecret() is %d\n", b.getSecret());
	// this next line will not compile... compiler enforces access restriction at compile
	// time...
	// a.secret=7;
	setSecret(&a, 7, false);
	setSecret(&b, 7, true);
	printf("a.getSecret() is %d\n", a.getSecret());
	printf("b.getSecret() is %d\n", b.getSecret());
	return EXIT_SUCCESS;
}
