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

class I { public:
	virtual ~I() { }
	virtual void doIt() = 0;
};

class A : public I { public:
	~A() { cout << "A dtor" << '\n'; }
	/*virtual*/ void doIt() { cout << 'A'; }
};

class D : public I {
	I* wrappee;
public:
	D( I* inner )			 { wrappee = inner; }
	~D()						{ delete wrappee; }
	/*virtual*/ void doIt() { wrappee->doIt(); }
};

class X : public D { public:
	X( I* core ) : D(core) { }
	~X() { cout << "X dtor" << "	"; }
	/*virtual*/ void doIt() { D::doIt(); cout << 'X'; }
};

class Y : public D { public:
	Y( I* core ) : D(core) { }
	~Y() { cout << "Y dtor" << "	"; }
	/*virtual*/ void doIt() { D::doIt(); cout << 'Y'; }
};

class Z : public D { public:
	Z( I* core ) : D(core) { }
	~Z() { cout << "Z dtor" << "	"; }
	/*virtual*/ void doIt() { D::doIt(); cout << 'Z'; }
};

int main() {
	I* anX = new X( new A );
	I* anXY = new Y( new X( new A ) );
	I* anXYZ = new Z( new Y( new X( new A ) ) );
	anX->doIt();	 cout << '\n';
	anXY->doIt();	cout << '\n';
	anXYZ->doIt(); cout << '\n';
	delete anX;	delete anXY;	delete anXYZ;
	return 0;
}
