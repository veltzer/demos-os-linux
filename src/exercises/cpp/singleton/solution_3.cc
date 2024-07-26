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

#include <firstinclude.h>
#include "singleton_3.hh"
#include <cassert>
#include <memory>

Singleton* Singleton::instance=nullptr;

mutex Singleton::m;

Singleton::Singleton() {}

Singleton& Singleton::get_instance() {
	if(instance==nullptr) {
		m.lock();
		if(instance==nullptr) {
			instance=new Singleton();
		}
		m.unlock();
	}
	return *instance;
}

int main() {
	Singleton& s1=Singleton::get_instance();
	Singleton& s2=Singleton::get_instance();
	assert(std::addressof(s1) == std::addressof(s2));
}
