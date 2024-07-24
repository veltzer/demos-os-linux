#include <iostream>

using namespace std;

void foo(
	int x1,
	int x2,
	int x3=3,
	int x4=4,
	int x5=5
	) {
	cout << "x1 is " << x1 << endl;
	cout << "x2 is " << x2 << endl;
	cout << "x3 is " << x3 << endl;
	cout << "x4 is " << x4 << endl;
	cout << "x5 is " << x5 << endl;
	cout << "finished" << endl;
}


int main() {
	foo(5, 6, 7);
	return EXIT_SUCCESS;
}
