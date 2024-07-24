#include <iostream>

using namespace std;

void print_my_value(const int* p) {
	cout << "my value is " << *p << endl;
}

int main() {
	const int i=5;
	int* p=(int*)&i;
	*p=7;
	cout << "i is " << i << endl;
	print_my_value(&i);
	return EXIT_SUCCESS;
}
