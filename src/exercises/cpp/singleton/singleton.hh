#pragma once
#include <mutex>
using namespace std;

class Singleton {
	private:
		double a, b;
		Singleton(double ia, double ib);
		static Singleton* instance;
		static mutex m;
	public:
		static Singleton* get_instance();
};
