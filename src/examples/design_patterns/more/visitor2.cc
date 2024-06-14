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

class Visitor { public:

	virtual void visit( class Primitive*, class Component* ) = 0;
	virtual void visit( class Composite*, Component* ) = 0;
};

class Component {
	int value;
public:
	Component( int val )	 { value = val; }
	virtual void traverse() { cout << value << " "; }
	// Having add() here sacrifices safety, but it supports transparency
	// virtual void add( Component* ) { }
	virtual void accept( Visitor&, Component* ) = 0;
};

class Primitive : public Component { public:
	Primitive( int val ) : Component( val ) { }
	/*virtual*/ void accept( Visitor& v, Component* c ) { v.visit( this, c ); }
};

class Composite : public Component {
	vector<Component*> children;
public:
	Composite( int val ) : Component( val ) { }
	void add( Component* ele ) { children.push_back( ele ); }
	/*virtual*/ void accept( Visitor& v, Component* c ) { v.visit( this, c ); }
	/*virtual*/ void traverse() {
		Component::traverse();
		for (unsigned int i=0; i < children.size(); i++)
			children[i]->traverse();
} };
////////////////////////////////////////////////////////////////////////////////////



class AddVisitor : public Visitor { public:
	/*virtual*/ void visit( Primitive*, Component* ) {/* does not make sense */}
	/*virtual*/ void visit( Composite* node, Component* c ) { node->add( c ); }
};

int main() {
	Component* nodes[3];
	// The type of Composite* is "lost" when the object is assigned to a
	// Component*
	nodes[0] = new Composite(1);
	nodes[1] = new Composite(2);
	nodes[2] = new Composite(3);

	// If add() were in class Component, this would work
	//	 nodes[0]->add( nodes[1] );
	// If it is NOT in Component, and only in Composite, you get the error -
	//	 no member function `Component::add(Component *)' defined

	// Instead of sacrificing safety, we use a Visitor to support add()
	AddVisitor addVisitor;
	nodes[0]->accept( addVisitor, nodes[1] );
	nodes[0]->accept( addVisitor, nodes[2] );
	nodes[0]->accept( addVisitor, new Primitive(4) );
	nodes[1]->accept( addVisitor, new Primitive(5) );
	nodes[1]->accept( addVisitor, new Primitive(6) );
	nodes[2]->accept( addVisitor, new Primitive(7) );

	for (int i=0; i < 3; i++) {
		nodes[i]->traverse();
		cout << endl;
	}
	return 0;
}

