#pragma once

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
