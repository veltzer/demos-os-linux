#include <boost/pool/pool.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <stdexcept>

class Person {
private:
    std::string name_;
    int age_;

public:
    Person() : name_(""), age_(0) {}
    
    const std::string& getName() const { return name_; }
    int getAge() const { return age_; }
    
    void setName(const std::string& name) { name_ = name; }
    void setAge(int age) { age_ = age; }
    
    void reset() {
        name_.clear();
        age_ = 0;
    }
    
    void display() const {
        std::cout << "Person: " << name_ << ", Age: " << age_ << std::endl;
    }
};

class PersonPool {
private:
    boost::pool<> memory_pool_;
    std::vector<Person*> available_;
    std::mutex mutex_;
    
public:
    PersonPool(size_t count) : memory_pool_(sizeof(Person)) {
        // Pre-allocate all objects
        for (size_t i = 0; i < count; ++i) {
            void* mem = memory_pool_.malloc();
            Person* person = new(mem) Person();  // Placement new
            available_.push_back(person);
        }
    }
    
    ~PersonPool() {
        std::lock_guard<std::mutex> lock(mutex_);
        // Call destructors for all objects
        for (Person* p : available_) {
            p->~Person();
        }
    }
    
    Person* get() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (available_.empty()) {
            throw std::runtime_error("Pool exhausted");
        }
        
        Person* person = available_.back();
        available_.pop_back();
        return person;
    }
    
    void put(Person* person) {
        if (!person) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        person->reset();
        available_.push_back(person);
    }
    
    size_t available_count() const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(mutex_));
        return available_.size();
    }
};

int main() {
    // Create pool with exactly 100 pre-allocated Person objects
    PersonPool pool(100);
    
    std::cout << "Pool created with " << pool.available_count() << " objects\n";
    
    // Get objects from pool
    Person* p1 = pool.get();
    Person* p2 = pool.get();
    
    std::cout << "After getting 2 objects: " << pool.available_count() << " available\n";
    
    // Use the objects
    p1->setName("Alice");
    p1->setAge(25);
    
    p2->setName("Bob");
    p2->setAge(30);
    
    p1->display();
    p2->display();
    
    // Return objects to pool
    pool.put(p1);
    pool.put(p2);
    
    std::cout << "After returning objects: " << pool.available_count() << " available\n";
    
    // Test pool exhaustion
    std::vector<Person*> persons;
    try {
        // Get all 100 objects
        for (int i = 0; i < 100; ++i) {
            persons.push_back(pool.get());
        }
        std::cout << "Successfully got all 100 objects\n";
        
        // Try to get one more (should throw)
        Person* extra = pool.get();
        std::cout << "ERROR: Should not reach here!\n";
	extra->display();
        
    } catch (const std::runtime_error& e) {
        std::cout << "Expected exception: " << e.what() << std::endl;
    }
    
    // Return all objects
    for (Person* p : persons) {
        pool.put(p);
    }
    
    std::cout << "Final count: " << pool.available_count() << " available\n";
    
    return 0;
}
