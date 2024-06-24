#include <iostream>

using namespace std;

class Base {
public:
    Base() {}
    ~Base() { cout << "Base destructor" << endl; }
};

class Derived : public Base {
public:
    Derived() {}
    ~Derived() { cout << "Derived destructor" << endl; }
};

int main() {
    Base* b = new Derived();
    delete b;  // Only calls Base destructor!
    return 0;
}
