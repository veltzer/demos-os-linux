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
#include <cstdlib>	// for EXIT_SUCCESS

using namespace std;

/*
 * An example of passing object and method and applying the method
 * on the object...
 * This is sometimes called a functor (object+method). You can find
 * similar approaches in boost's bind function.
 */

class A{
public:
	int val;
	void run() {
		cout << val << endl;
	}
};

void apply(A& a, void(A::*method)()) {
	(a.*method)();
}

int main() {
	A a;
	a.val=7;
	apply(a, &A::run);
	return EXIT_SUCCESS;
}
