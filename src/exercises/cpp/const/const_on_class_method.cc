#include <iostream>

using namespace std;

class Rectangle {
	private:
		double a,b;
		mutable double precalc_area=0;
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
			precalc_area=0;
		}
		void set_b(double ib) {
			b=ib;
			precalc_area=0;
		}
		double area() const {
			if(precalc_area==0) {
				precalc_area=a*b;
			}
			return precalc_area;
		}
};

int main() {
	const Rectangle r(5,6);
	cout << r.get_a() << endl;
	cout << "the area is " << r.area() << endl;
	// the next line will cause a compile error
	// r.set_a(7);
	return EXIT_SUCCESS;
}
