#include <iostream>
#include <utility>
#include <vector>
#include <string>

class MyClass {
public:
    MyClass(std::string str) : data(std::move(str)) {}
    MyClass(const MyClass& other) : data(other.data) {
        std::cout << "Copy constructor called" << std::endl;
    }
    MyClass(MyClass&& other) : data(std::move(other.data)) {
        std::cout << "Move constructor called" << std::endl;
    }
    ~MyClass() {
        std::cout << "Destructor called for: " << data << std::endl;
    }

private:
    std::string data;
};

int main() {
    std::string str = "Hello";
    std::vector<MyClass> vec;

    // Copy construction
    vec.push_back(MyClass(str));
    std::cout << "After copy construction" << std::endl;

    // Move construction
    vec.push_back(MyClass(std::move(str)));
    std::cout << "After move construction" << std::endl;

    std::cout << "Vector size: " << vec.size() << std::endl;

    return 0;
}
