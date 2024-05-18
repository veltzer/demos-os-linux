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
#include <iostream>
#include <stdlib.h>
using namespace std;

// Base class
class Shape{
public:
	void draw() {
		cout << "Drawing a shape..." << endl;
	}
};

// Inherited class
class Circle: public Shape {
public:
	void draw() {
		Shape::draw();
		cout << "Drawing a circle..." << endl;
	}
};

// Composition class
class Square{
public:
	void draw() {
		shape.draw();
		cout << "Drawing a square..." << endl;
	}

private:
	Shape shape;
};

int main() {
	Circle c;
	c.draw();	// Calling derived
	Square s;
	s.draw();	// Calls base via composition
	return EXIT_SUCCESS;
}
