/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2024 Mark Veltzer <mark.veltzer@gmail.com>
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

#include <iostream>
#include <memory>
#include <vector>

using namespace std;

class Resource {
public:
	Resource(int value) : data(value) {
		cout << "Resource constructed with value " << data << endl;
	}
	~Resource() {
		cout << "Resource destructed with value " << data << endl;
	}
	int getData() const { return data; }
private:
	int data;
};

void useResource(shared_ptr<Resource> ptr) {
	cout << "Using resource with value: " << ptr->getData() << endl;
}

int main() {
	// Basic usage
	{
		cout << "1. Basic shared_ptr usage:" << endl;
		auto ptr1 = make_shared<Resource>(42);
		cout << "Reference count: " << ptr1.use_count() << endl;
		
		{
			auto ptr2 = ptr1;
			cout << "Reference count after shared: " << ptr1.use_count() << endl;
		}
		
		cout << "Reference count after ptr2 goes out of scope: " << ptr1.use_count() << endl;
	}
	cout << endl;

	// Passing to functions
	{
		cout << "2. Passing shared_ptr to functions:" << endl;
		auto ptr = make_shared<Resource>(10);
		useResource(ptr);
	}
	cout << endl;

	// Using in containers
	{
		cout << "3. Using shared_ptr in containers:" << endl;
		vector<shared_ptr<Resource>> resources;
		resources.push_back(make_shared<Resource>(1));
		resources.push_back(make_shared<Resource>(2));
		resources.push_back(make_shared<Resource>(3));
		
		for(const auto& res : resources) {
			cout << "Resource value: " << res->getData() << endl;
		}
	}
	cout << endl;

	// Custom deleter
	{
		cout << "4. Using custom deleter:" << endl;
		auto customDeleter = [](Resource* p) {
			cout << "Custom deleting resource with value " << p->getData() << endl;
			delete p;
		};
		shared_ptr<Resource> ptr(new Resource(100), customDeleter);
	}
	cout << endl;

	return 0;
}
