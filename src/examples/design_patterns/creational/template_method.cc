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
#include <iostream>

using namespace std;

class A {
public:
	// 1. Standardize the skeleton of an algorithm in a "template" method
	void findSolution() {
		stepOne();
		stepTwo();
		stepThr();
		stepFor();
	}
protected:
	virtual void stepFor() { cout << "A.stepFor" << '\n'; }
private:
	// 2. Common implementations of individual steps are defined in base class
	void stepOne() { cout << "A.stepOne" << '\n'; }
	// 3. Steps requiring peculiar impls are "placeholders" in the base class
	virtual void stepTwo() = 0;
	virtual void stepThr() = 0;
};

class B : public A {
	// 4. Derived classes can override placeholder methods
	// 1. Standardize the skeleton of an algorithm in a "template" method
	/*virtual*/ void stepThr() {
		step3_1();
		step3_2();
		step3_3();
	}
	// 2. Common implementations of individual steps are defined in base class
	void step3_1() { cout << "B.step3_1" << '\n'; }
	// 3. Steps requiring peculiar impls are "placeholders" in the base class
	virtual void step3_2() = 0;
	void step3_3() { cout << "B.step3_3" << '\n'; }
};

class C : public B {
	// 4. Derived classes can override placeholder methods
	/*virtual*/ void stepTwo() { cout << "C.stepTwo" << '\n'; }
	void step3_2() { cout << "C.step3_2" << '\n'; }
	// 5. Derived classes can override implemented methods
	// 6. Derived classes can override and "call back to" base class methods
	/*virtual*/ void stepFor() {
		cout << "C.stepFor" << '\n';
		A::stepFor();
	}
};

int main() {
	C algorithm;
	algorithm.findSolution();
	return 0;
}
