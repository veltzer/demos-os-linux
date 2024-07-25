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

#include <iostream>

using namespace std;

class Rectangle {
	private:
		double a,b;
		mutable double precalc_area=0;
	public:
		Rectangle(double ia, double ib):a(ia), b(ib) {
		}
		double get_a() const {
			return a;
		}
		double get_b() const {
			return b;
		}
		void set_a(double ia) {
			a=ia;
			precalc_area=0;
		}
		void set_b(double ib) {
			b=ib;
			precalc_area=0;
		}
		double area() const {
			if(precalc_area==0) {
				precalc_area=a*b;
			}
			return precalc_area;
		}
};

int main() {
	const Rectangle r(5,6);
	cout << r.get_a() << endl;
	cout << "the area is " << r.area() << endl;
	// the next line will cause a compile error
	// r.set_a(7);
	return EXIT_SUCCESS;
}
