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
