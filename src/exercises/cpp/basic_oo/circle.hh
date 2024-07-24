#pragma once

#include <iostream>

class Circle {
    private:
        double radius, x, y;
    public:
        Circle(double radius, double x, double y);

        // getters and setters
        double get_radius() const {
            return radius;
        }        
        void set_radius(double iradius) {
            radius = iradius;
        }
	double get_x() const {
		return x;
	}
	double get_y() const {
		return y;
	}

        bool inside(double, double);

        friend std::ostream& operator<<(std::ostream& os, const Circle& obj);
};
