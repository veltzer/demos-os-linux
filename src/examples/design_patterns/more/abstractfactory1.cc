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

//Abstract Factory Example

#include <firstinclude.h>
#include <iostream>

using namespace std;

class Shape {
public:
	Shape() { id_ = total_++; }
	virtual void draw() = 0;
	virtual ~Shape() {}
protected:
	int	id_;
	static int total_;
};
int Shape::total_ = 0;

class Circle : public Shape
{
public:
	void draw() { cout << "circle " << id_ << ": draw" << endl; }
};

class Square : public Shape
{
public:
	void draw() { cout << "square " << id_ << ": draw" << endl; }
};

class Ellipse : public Shape
{
public:
	void draw() { cout << "ellipse " << id_ << ": draw" << endl; }
};
class Rectangle : public Shape
{
public:
	void draw() { cout << "rectangle " << id_ << ": draw" << endl; }
};

class Factory
{
public:
	virtual Shape* createCurvedInstance()	= 0;
	virtual Shape* createStraightInstance() = 0;
	virtual ~Factory() {};
};

class SimpleShapeFactory : public Factory
{
public:
	Shape* createCurvedInstance()	{ return new Circle; }
	Shape* createStraightInstance() { return new Square; }
};

class RobustShapeFactory : public Factory
{
public:
	Shape* createCurvedInstance()	{ return new Ellipse; }
	Shape* createStraightInstance() { return new Rectangle; }
};

int main() {
	Factory* factory = new SimpleShapeFactory;
	Shape*	 shapes[3];

	shapes[0] = factory->createCurvedInstance();
	shapes[1] = factory->createStraightInstance();
	shapes[2] = factory->createCurvedInstance();

	for(int i=0; i < 3; i++){
		shapes[i]->draw();
		delete shapes[i];
	}

	delete factory;
	factory = new RobustShapeFactory;

	shapes[0] = factory->createCurvedInstance();
	shapes[1] = factory->createStraightInstance();
	shapes[2] = factory->createCurvedInstance();

	for(int i=0; i < 3; i++)
		shapes[i]->draw();

	return 0;
}

