# Adapter Exercise

Here are two classes:

```c++
// Create an interface for the target class
class ModernInterface {
public:
    virtual void newRequest() = 0;
    virtual ~ModernInterface() = default;
};

// Create a class that needs to be adapted
class LegacyClass {
public:
    void oldRequest() {
        // Legacy implementation
        cout << "LegacyClass: Handling old request." << endl;
    }
};

// TODO: Implement an Adapter class named 'Adapter' that:
// 1. Inherits from ModernInterface
// 2. Contains an instance of LegacyClass
// 3. Implements the newRequest() method to call oldRequest() on the LegacyClass instance

// Test the adapter
int main() {
    // TODO: Create an instance of the Adapter class
    // TODO: Call newRequest() on the adapter instance
    return 0;
}
```

Please adapt the classes one to the other.
