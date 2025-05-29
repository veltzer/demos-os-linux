/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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

namespace Math {
	const double PI = 3.14159;

	double square(double x) {
		return x * x;
	}

	double cube(double x) {
		return x * x * x;
	}
}

namespace WrongMath {
	double square(double x) {
		return x + x;
	}
}

#include <iostream>

using namespace std;
using namespace Math;
using namespace WrongMath;

int main() {
	double result = WrongMath::square(5);
	double volume = cube(3);
	cout << "result is " << result << endl;
	cout << "volume is " << volume << endl;
	return 0;
}
