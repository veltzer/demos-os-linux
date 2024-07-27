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
#include "circle2.hh"

using namespace std;

Circle::Circle(const double iradius, const double ix, const double iy):radius(iradius), x(ix), y(iy) {
}

bool Circle::inside(const double ix, const double iy) const {
	double distance2 = (ix-x)*(ix-x) + (iy-y)*(iy-y);
	return distance2 <= radius*radius;
}

ostream& operator<<(ostream& os, const Circle& obj) {
	os << "radius is " << obj.get_radius() << endl;
	os << "x is " << obj.get_x() << endl;
	os << "y is " << obj.get_y() << endl;
	return os;
}

int main() {
}
