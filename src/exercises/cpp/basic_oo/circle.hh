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

#include <iostream>

class Circle {
	private:
		double radius, x, y;
	public:
		Circle(double radius, double x, double y);

		// getters and setters
		double get_radius() {
			return radius;
		}
		void set_radius(double iradius) {
			radius = iradius;
		}
	double get_x() {
		return x;
	}
	double get_y() {
		return y;
	}

	bool inside(double, double);

	friend std::ostream& operator<<(std::ostream& os, Circle& obj);
};
