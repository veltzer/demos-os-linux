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
#include <cstdlib>

using namespace std;

class Person {
	private:
		int age;
		bool married;
	public:
		Person(int iage, bool imarried) {
			age=iage;
			married=imarried;
		}
	friend ostream& operator<<(ostream& os, const Person& obj);
};

ostream& operator<<(ostream& os, const Person& obj) {
	os << "my age is " << obj.age << endl;
	os << "married status is " << obj.married << endl;
	return os;
}


int main() {
	int age = 25;
	bool allowed_to_drive = true;
	cout << "my age is " << age << endl;
	cout << "allowed to drive is " << allowed_to_drive << endl;
	Person p(29, true);
	cout << "person details are " << p << endl;
	return EXIT_SUCCESS;
}
