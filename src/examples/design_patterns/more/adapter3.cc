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

using namespace std;

class ExecuteInterface { public:						// 1. Specify the new i/f
	virtual ~ExecuteInterface() { }
	virtual void execute() = 0;
};

template <class TYPE>									// 2. Design a "wrapper" or
class ExecuteAdapter : public ExecuteInterface { //	 "adapter" class
public:
	ExecuteAdapter( TYPE* o, void (TYPE::*m)() ) { object = o; method =m; }
	~ExecuteAdapter()									 { delete object; }
	// 4. The adapter/wrapper "maps" the new to the legacy implementation
	void execute()				 /* the new */	{ (object->*method)(); }
private:
	TYPE* object;											 // ptr-to-object attribute
	void (TYPE::*method)();	 /* the old */		 // ptr-to-member-function
};																//	attribute

// The old: three totally incompatible classes	 // no common base class,
class Fea { public:										 // no hope of polymorphism
	~Fea()		{ cout << "Fea::dtor" << endl; }
	void doThis() { cout << "Fea::doThis()" << endl; }
};

class Feye { public:
	~Feye()		 { cout << "Feye::dtor" << endl; }
	void doThat() { cout << "Feye::doThat()" << endl; }
};

class Pheau { public:
	~Pheau()			 { cout << "Pheau::dtor" << endl; }
	void doTheOther() { cout << "Pheau::doTheOther()" << endl; }
};

/* the new is returned */ ExecuteInterface** initialize() {
	ExecuteInterface** array = new ExecuteInterface*[3]; /* the old is below */
	array[0] = new ExecuteAdapter<Fea>(	new Fea(),	&Fea::doThis		 );
	array[1] = new ExecuteAdapter<Feye>(new Feye(),	 &Feye::doThat		);
	array[2] = new ExecuteAdapter<Pheau>( new Pheau(),	&Pheau::doTheOther );
	return array;
}

int main() {
	ExecuteInterface** objects = initialize();

	for (int i=0; i < 3; i++) objects[i]->execute(); // 3. Client uses the new
																	//	 (polymporphism)
	for (int i=0; i < 3; i++)
		delete objects[i];
	delete objects;
	return 0;
}
