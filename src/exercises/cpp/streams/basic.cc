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
