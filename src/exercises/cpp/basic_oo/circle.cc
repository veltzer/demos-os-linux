#include "circle.hh"
#include <iostream>

using namespace std;

Circle::Circle(double ir, double ix, double iy) {
	radius = ir;
	x = ix;
	y = iy;
}

bool Circle::inside(double ix, double iy) {
	double distance2 = (ix-x)*(ix-x) + (iy-y)*(iy-y);
	return distance2 <= radius*radius;
}

std::ostream& operator<<(std::ostream& os, Circle& obj) {
	os << "radius is " << obj.get_radius() << endl;
	os << "x is " << obj.get_x() << endl;
	os << "y is " << obj.get_y() << endl;
	return os;
}

int main() {
}
