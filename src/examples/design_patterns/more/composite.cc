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
#include <vector>

using namespace std;

class Component { public: virtual void traverse() = 0; };

class Primitive : public Component {
	int value;
public:
	Primitive( int val ) { value = val; }
	void traverse()		{ cout << value << " "; }
};

class Composite : public Component {
	vector<Component*> children;
	int					 value;
public:
	Composite( int val )	 { value = val; }
	void add( Component* c ) { children.push_back( c ); }
	void traverse() {
		cout << value << " ";
		for (unsigned int i=0; i < children.size(); i++)
			 children[i]->traverse();
} };

class Row : public Composite { public:	 // Two different kinds of "con-
	Row( int val ) : Composite( val ) { }	// tainer" classes. Most of the
	void traverse() {							 // "meat" is in the Composite
		cout << "Row";							 // base class.
		Composite::traverse();
} };

class Column : public Composite { public:
	Column( int val ) : Composite( val ) { }
	void traverse() {
		cout << "Col";
		Composite::traverse();
} };

int main() {
	Row first( 1 );							// Row1
	Column second( 2 );						//	|
	Column third( 3 );							//	+-- Col2
	Row fourth( 4 );						//	|	|
	Row fifth( 5 );							//	|	+-- 7
	first.add( &second );						//	+-- Col3
	first.add( &third);						//	|	|
	third.add( &fourth );						//	|	+-- Row4
	third.add( &fifth);						//	|	|	|
	first.add(new Primitive( 6 ) );			//	|	|	+-- 9
	second.add(new Primitive( 7 ) );			//	|	+-- Row5
	third.add(new Primitive( 8 ) );			//	|	|	|
	fourth.add(new Primitive( 9 ) );			//	|	|	+-- 10
	fifth.add(new Primitive(10 ) );			//	|	+-- 8
	first.traverse(); cout << '\n';		 //	+-- 6
	return 0;
}

