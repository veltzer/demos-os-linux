#include "singleton.hh"


Singleton* Singleton::instance=nullptr;

mutex Singleton::m;

Singleton::Singleton(double ia, double ib):a(ia), b(ib) {}

Singleton* Singleton::get_instance() {
	if(instance==nullptr) {
		m.lock();
		if(instance==nullptr) {
			instance=new Singleton(4,5);
		}
		m.unlock();
	}
	return instance;
}

int main() {

}
