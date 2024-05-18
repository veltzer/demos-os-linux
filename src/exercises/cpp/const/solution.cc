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
#include <cmath>
#include <stdlib.h>
#include <iostream>

class Rectangle{
private:
	double width;
	double height;

public:
	Rectangle(const double w, const double h) : width(w), height(h) {
		std::cout << getArea() << std::endl;
	}

	double getArea() const{
		return width * height;
	}

	double getWidth() const{
		return width;
	}

	double getHeight() const{
		return height;
	}

	void setWidth(const double w) {
		width = w;
	}

	void setHeight(const double h) {
		height = h;
	}

	double getDiagonalLength() const{
		return sqrt(width * width + height * height);
	}

	double getCircumference() const{
		return 2 * (width + height);
	}

	bool is_intersectiog(const Rectangle& r __attribute__((unused))) {
		// TBD
		return true;
	}
};

int main() {
	const Rectangle r(5, 3);
	std::cout << "Area: " << r.getArea() << std::endl;
	std::cout << "Width: " << r.getWidth() << std::endl;
	std::cout << "Height: " << r.getHeight() << std::endl;
	std::cout << "Diagonal Length: " << r.getDiagonalLength() << std::endl;
	std::cout << "Circumference: " << r.getCircumference() << std::endl;
	return EXIT_SUCCESS;
}
