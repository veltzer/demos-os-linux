#include "circle2.hh"

using namespace std;

Circle::Circle(const double iradius, const double ix, const double iy):radius(iradius), x(ix), y(iy) {
}

bool Circle::inside(const double ix, const double iy) const {
	double distance2 = (ix-x)*(ix-x) + (iy-y)*(iy-y);
	return distance2 <= radius*radius;
}

std::ostream& operator<<(std::ostream& os, const Circle& obj) {
	os << "radius is " << obj.get_radius() << endl;
	os << "x is " << obj.get_x() << endl;
	os << "y is " << obj.get_y() << endl;
	return os;
}

int main() {
}
