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

typedef int Coordinate;
typedef int Dimension;

/////////////////////////// Desired interface ///////////////////////////
class Rectangle {
public:
	virtual void draw() = 0;
};

/////////////////////////// Legacy component ///////////////////////////
class LegacyRectangle {
public:
	LegacyRectangle( Coordinate x1, Coordinate y1,
			Coordinate x2, Coordinate y2 ) {
		x1_ = x1; y1_ = y1; x2_ = x2; y2_ = y2;
		cout << "LegacyRectangle: create. (" << x1_ << "," << y1_
			<< ") => (" << x2_ << "," << y2_ << ")" << endl; }
	void oldDraw() {
		cout << "LegacyRectangle: oldDraw. (" << x1_ << "," << y1_
			<< ") => (" << x2_ << "," << y2_ << ")" << endl; }
private:
	Coordinate x1_;
	Coordinate y1_;
	Coordinate x2_;
	Coordinate y2_;
};

/////////////////////////// Adapter wrapper ///////////////////////////
class RectangleAdapter : public Rectangle, private LegacyRectangle {
public:
	RectangleAdapter( Coordinate x, Coordinate y, Dimension w, Dimension h )
			: LegacyRectangle( x, y, x+w, y+h ) {
		cout << "RectangleAdapter: create. (" << x << "," << y
			<< "), width = " << w << ", height = " << h << endl; }
	virtual void draw() {
		cout << "RectangleAdapter: draw." << endl;
		oldDraw(); }
};

int main() {
	Rectangle* r = new RectangleAdapter( 120, 200, 60, 40 );
	r->draw();
	return 0;
}

