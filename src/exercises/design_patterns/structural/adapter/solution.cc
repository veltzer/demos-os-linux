#include <iostream>

// Interface for the target class
class ModernInterface {
public:
    virtual void newRequest() = 0;
    virtual ~ModernInterface() = default;
};

// Class that needs to be adapted
class LegacyClass {
public:
    void oldRequest() {
        std::cout << "LegacyClass: Handling old request." << std::endl;
    }
};

// Adapter class
class Adapter : public ModernInterface {
private:
    LegacyClass legacyObject;

public:
    void newRequest() override {
        std::cout << "Adapter: Converting new request to old request." << std::endl;
        legacyObject.oldRequest();
    }
};

// Test the adapter
int main() {
    ModernInterface* adapter = new Adapter();
    adapter->newRequest();
    
    delete adapter;
    return 0;
}
