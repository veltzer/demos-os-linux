#include <iostream>
#include <memory>
#include <vector>

class Resource {
public:
    Resource(int value) : data(value) {
        std::cout << "Resource constructed with value " << data << std::endl;
    }
    ~Resource() {
        std::cout << "Resource destructed with value " << data << std::endl;
    }
    int getData() const { return data; }
private:
    int data;
};

void useResource(std::shared_ptr<Resource> ptr) {
    std::cout << "Using resource with value: " << ptr->getData() << std::endl;
}

int main() {
    // Basic usage
    {
        std::cout << "1. Basic shared_ptr usage:" << std::endl;
        auto ptr1 = std::make_shared<Resource>(42);
        std::cout << "  Reference count: " << ptr1.use_count() << std::endl;
        
        {
            auto ptr2 = ptr1;
            std::cout << "  Reference count after shared: " << ptr1.use_count() << std::endl;
        }
        
        std::cout << "  Reference count after ptr2 goes out of scope: " << ptr1.use_count() << std::endl;
    }
    std::cout << std::endl;

    // Passing to functions
    {
        std::cout << "2. Passing shared_ptr to functions:" << std::endl;
        auto ptr = std::make_shared<Resource>(10);
        useResource(ptr);
    }
    std::cout << std::endl;

    // Using in containers
    {
        std::cout << "3. Using shared_ptr in containers:" << std::endl;
        std::vector<std::shared_ptr<Resource>> resources;
        resources.push_back(std::make_shared<Resource>(1));
        resources.push_back(std::make_shared<Resource>(2));
        resources.push_back(std::make_shared<Resource>(3));
        
        for (const auto& res : resources) {
            std::cout << "  Resource value: " << res->getData() << std::endl;
        }
    }
    std::cout << std::endl;

    // Custom deleter
    {
        std::cout << "4. Using custom deleter:" << std::endl;
        auto customDeleter = [](Resource* p) {
            std::cout << "  Custom deleting resource with value " << p->getData() << std::endl;
            delete p;
        };
        std::shared_ptr<Resource> ptr(new Resource(100), customDeleter);
    }
    std::cout << std::endl;

    return 0;
}
