#include <iostream>

class Vector2D {
private:
    double x, y;

public:
    Vector2D(double x = 0.0, double y = 0.0) : x(x), y(y) {}

    // Overloading the + operator
    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }

    // For printing the vector
    friend std::ostream& operator<<(std::ostream& os, const Vector2D& v);
};

// Output stream operator overloading
std::ostream& operator<<(std::ostream& os, const Vector2D& v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}

int main() {
    Vector2D v1(1.0, 2.0);
    Vector2D v2(3.0, 4.0);

    Vector2D v3 = v1 + v2;

    std::cout << "v1 = " << v1 << std::endl;
    std::cout << "v2 = " << v2 << std::endl;
    std::cout << "v1 + v2 = " << v3 << std::endl;

    return 0;
}
