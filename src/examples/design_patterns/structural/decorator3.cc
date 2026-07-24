/*
 * This file is part of the demos-os-linux package.
 * Copyright (C) 2011-2026 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-os-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-os-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-os-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <iostream>

using namespace std;

class Widget { public: virtual ~Widget() {} virtual void draw() = 0; }; // 1. "lowest common denom"

class TextField : public Widget {						 // 3. "Core" class & "isa"
	int width, height;
public:
	TextField( int w, int h ) { width = w; height = h; }
	/*virtual*/ void draw() override { cout << "TextField: " << width << ", " << height << '\n'; }
};
																	 // 2. 2nd level base class
class Decorator : public Widget {						 // 3. "isa" relationship
	Widget* wid;												 // 4. "hasa" relationship
public:
	explicit Decorator( Widget* w ) { wid = w; }
	Decorator( const Decorator& ) = delete;
	Decorator& operator=( const Decorator& ) = delete;
	~Decorator() override { delete wid; }
	/*virtual*/ void draw() override { wid->draw(); }			// 5. Delegation
};

class BorderDecorator : public Decorator { public: // 6. Optional embellishment
	explicit BorderDecorator( Widget* w ) : Decorator( w ) { }
	/*virtual*/ void draw() override {
		Decorator::draw();									 // 7. Delegate to base class
		cout << "	BorderDecorator" << '\n';			//	 and add extra stuff
} };

class ScrollDecorator : public Decorator { public: // 6. Optional embellishment
	explicit ScrollDecorator( Widget* w ) : Decorator( w ) { }
	/*virtual*/ void draw() override {
		Decorator::draw();									 // 7. Delegate to base class
		cout << "	ScrollDecorator" << '\n';			//	 and add extra stuff
} };

int main() {
	// 8. Client has the responsibility to compose desired configurations
	Widget* aWidget = new BorderDecorator(
								new BorderDecorator(
									new ScrollDecorator(
										new TextField( 80, 24 ))));
	aWidget->draw();
	delete aWidget;
	return 0;
}

