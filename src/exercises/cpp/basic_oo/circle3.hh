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

#pragma once

#include <firstinclude.h>
#include <iostream>

class Circle {
	private:
		double radius, x, y;
	public:
		Circle(const double radius, const double x, const double y);

	// getters and setters
	double get_radius() const {
		return radius;
	}
	void set_radius(const double iradius) {
		radius = iradius;
	}
	double get_x() const {
		return x;
	}
	void set_x(const double ix) {
		x=ix;
	}
	double get_y() const {
		return y;
	}
	void set_y(const double iy) {
		y=iy;
	}

	bool inside(const double, const double) const;

	friend std::ostream& operator<<(std::ostream& os, const Circle& obj);

	bool is_concentric(const Circle& c) const;

	const Circle& concentric_overlap(const Circle& c) const;
};
