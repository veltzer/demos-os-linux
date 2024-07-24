#include "circle3.hh"

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

bool Circle::is_concentric(const Circle& c) const {
	return x==c.x && y==c.y;
}

const Circle& Circle::concentric_overlap(const Circle& c) const {
	if(!this->is_concentric(c)) {
		throw new string("not concentinc");
	}
	if(radius<c.radius) {
		return *this;
	}
	return c;
}

/*
Circle& Circle::concentric_overlap(const Circle& c) const {
        if(!this->is_concentric(c)) {
                throw new string("not concentinc");
        }
        if(radius<c.radius) {
                return *new Circle(radius, x, y);
        }
        return *new Circle(c.radius, c.x, c.y);
}
*/

int main() {
	Circle& c=*new Circle(5, 0, 0);
	Circle& c2=*new Circle(3, 0, 0);
	cout << c;
	cout << c2;
	const Circle& c3=c.concentric_overlap(c2);
	cout << c3;
	// next line will not compile
	//c3.set_radius(6.7);
}
