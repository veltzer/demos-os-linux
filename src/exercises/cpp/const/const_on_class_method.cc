#include <iostream>

using namespace std;

class Rectangle {
	private:
		double a,b;
	public:
		Rectangle(double ia, double ib):a(ia), b(ib) {
		}
		double get_a() const {
			return a;
		}
		double get_b() const {
			return b;
		}
		void set_a(double ia) {
			a=ia;
		}
};

int main() {
	const Rectangle r(5,6);
	cout << r.get_a() << endl;
	// r.set_a(7);
	return EXIT_SUCCESS;
}
