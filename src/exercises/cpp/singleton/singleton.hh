#pragma once

class Singleton {
	private:
		double a, b;
		Singleton(double ia, double ib);
		static Singleton* instance;
	public:
		static Singleton* get_instance();
};
