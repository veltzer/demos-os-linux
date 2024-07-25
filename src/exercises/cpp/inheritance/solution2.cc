#include <iostream>
#include <cstdlib>
#include <vector>

using namespace std;


class Shape {
	public:
		virtual void print_me() const=0;
};

class Rectangle: public Shape {
	private:
		double a, b;
	public:
		Rectangle(double ia, double ib):a(ia), b(ib) {}
		virtual void print_me() const override {
			cout << "Rectangle" << endl;
			cout << "a is " << a << endl;
			cout << "b is " << b << endl;
		}
};

class Circle: public Shape {
	private:
		double r, x, y;
	public:
		Circle(double ir, double ix, double iy):r(ir), x(ix), y(iy) {}
		void print_me() const override {
			cout << "Circle" << endl;
			cout << "r is " << r << endl;
			cout << "x is " << x << endl;
			cout << "y is " << y << endl;
		}
};

class Triangle: public Shape {
	private:
		double a, b, c;
	public:
		Triangle(double ia, double ib, double ic):a(ia), b(ib), c(ic) {}
		void print_me() const override {
			cout << "Triangle" << endl;
			cout << "a is " << a << endl;
			cout << "b is " << b << endl;
			cout << "c is " << c << endl;
		}
};


int main() {
	vector<reference_wrapper<Shape>> v;
	while(true) {
		cout << "1. add rectangle" << endl;
		cout << "2. add circle" << endl;
		cout << "3. add triangle" << endl;
		cout << "4. stop" << endl;
		int choice;
		cin >> choice;
		if(choice==4) {
			break;
		}
		double a, b, c, r, x, y;
		switch(choice) {
			case 1:
				cin >> a >> b;
				v.push_back(*new Rectangle(a, b));
				break;
			case 2:
				cin >> r >> x >> y;
				v.push_back(*new Circle(r, x, y));
				break;
			case 3:
				cin >> a >> b >> c;
				v.push_back(*new Triangle(a, b, c));
				break;
		}
	}
	for (size_t i = 0; i < v.size(); i++) {
		v[i].get().print_me();
	}
	return EXIT_SUCCESS;
}
