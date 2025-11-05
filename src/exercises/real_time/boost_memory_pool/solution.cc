/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <boost/pool/object_pool.hpp>
#include <iostream>
#include <string>

using namespace std;

class Person {
private:
    string name_;
    int age_;

public:
    Person() : name_(""), age_(0) {
        cout << "Person constructor is called" << endl;
    }

    // Getters
    const string& getName() const { return name_; }
    int getAge() const { return age_; }

    // Setters
    void setName(const string& name) { name_ = name; }
    void setAge(int age) { age_ = age; }

    void display() const {
        cout << "Person: " << name_ << ", Age: " << age_ << endl;
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
