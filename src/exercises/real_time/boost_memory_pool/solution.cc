#include <boost/pool/object_pool.hpp>
#include <iostream>
#include <string>

class Person {
private:
    std::string name_;
    int age_;

public:
    Person() : name_(""), age_(0) {
        std::cout << "Person constructor is called" << std::endl;
    }
    
    // Getters
    const std::string& getName() const { return name_; }
    int getAge() const { return age_; }
    
    // Setters
    void setName(const std::string& name) { name_ = name; }
    void setAge(int age) { age_ = age; }
    
    void display() const {
        std::cout << "Person: " << name_ << ", Age: " << age_ << std::endl;
    }
};

int main() {
    // Create pool of 32 Person objects
    boost::object_pool<Person> pool;
    
    // Get Person objects from pool
    Person* p1 = pool.construct();
    Person* p2 = pool.construct();
    
    // Use the objects
    p1->setName("Alice");
    p1->setAge(25);
    
    p2->setName("Bob");
    p2->setAge(30);
    
    p1->display();
    p2->display();
    
    // Return objects to pool (destroy them)
    pool.destroy(p1);
    pool.destroy(p2);
 
    // Get new objects (memory is reused)
    Person* p3 = pool.construct();
    p3->setName("Charlie");
    p3->setAge(35);
    p3->display();
    
    pool.destroy(p3);
    return EXIT_SUCCESS;
}
