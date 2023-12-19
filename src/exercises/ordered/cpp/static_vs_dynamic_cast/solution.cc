/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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

class Shape {
public:
	virtual double getArea() { return 0; }
};

class Circle : public Shape {
private:
	double radius;
public:
	Circle(double radius) { this->radius = radius; }
	double getArea() override { return 3.14 * radius * radius; }
};

class Square : public Shape {
private:
	double side;
public:
	Square(double side) { this->side = side; }
	double getArea() override { return side * side; }
};

class Triangle : public Shape {
private:
	double base;
	double height;
public:
	Triangle(double base, double height) {
		this->base = base;
		this->height = height;
	}
	double getArea() override { return 0.5 * base * height; }
};

void printArea(Shape* shape) {
	cout << shape->getArea() << endl;
}

int main(int argc, char** argv, char** envp) {

	Circle c(5);
	printArea(&c);

	Square s(4);
	printArea(&s);

	Triangle t(2, 3);
	printArea(&t);

	// Static cast
	// does not compile
	//Circle* sc = static_cast<Circle*>(&s);
	//
	Shape& sh=t;

	// Dynamic cast
	// does not compile
	//Circle* dc = dynamic_cast<Circle*>(&s); // returns NULL
	Circle* dc = dynamic_cast<Circle*>(&sh); // returns NULL

	if(dc != nullptr) {
		cout << dc->getArea() << endl;
	} else {
		cout << "Null pointer" << endl;
	}

	return 0;
}
