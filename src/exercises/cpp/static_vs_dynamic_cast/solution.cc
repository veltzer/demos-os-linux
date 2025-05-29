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
#include <stdlib.h>
using namespace std;

class Shape{
public:
	virtual double getArea() {
		return 0;
	}
	// double getArea() { return 0; }
};

class Circle: public Shape {
private:
	double radius;

public:
	Circle(double radius) {
		this->radius = radius;
	}
	double getArea() override{
		return 3.14 * radius * radius;
	}
};

class Square: public Shape {
private:
	double side;

public:
	Square(double side) {
		this->side = side;
	}
	double getArea() override{
		return side * side;
	}
};

class Triangle: public Shape {
private:
	double base;
	double height;

public:
	Triangle(double base, double height) {
		this->base = base;
		this->height = height;
	}
	double getArea() override{
		return 0.5 * base * height;
	}
};

void printArea(Shape* shape) {
	cout << shape->getArea() << endl;
}

int main() {
	Circle circle(5);
	printArea(&circle);

	Square square(4);
	printArea(&square);

	Triangle triangle(2, 3);
	printArea(&triangle);

	Shape& shape=triangle;

	// Static cast
	// compiles because of the compiler stupidity
	Circle* c1 = static_cast<Circle*>(&shape);
	cout << c1 << endl;
	// does not compile
	// Circle* c2 = static_cast<Circle*>(&square);
	// cout << c2 << endl;

	// Dynamic cast
	// does not compile
	// Circle* c3 = dynamic_cast<Circle*>(&square); // returns NULL
	// cout << c3 << endl;
	Circle* c4 = dynamic_cast<Circle*>(&shape);	// returns NULL
	if(c4 != nullptr) {
		cout << c4->getArea() << endl;
	} else {
		cout << "Null pointer" << endl;
	}
	return EXIT_SUCCESS;
}
