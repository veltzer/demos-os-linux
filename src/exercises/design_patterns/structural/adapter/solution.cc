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
#include <iostream>
#include <stdlib.h>

// Interface for the target class
class ModernInterface {
public:
	virtual void newRequest() = 0;
	virtual ~ModernInterface() = default;
};

// Class that needs to be adapted
class LegacyClass {
public:
	void oldRequest() {
		std::cout << "LegacyClass: Handling old request." << std::endl;
	}
};

// Adapter class
class Adapter : public ModernInterface {
private:
	LegacyClass legacyObject;

public:
	void newRequest() override {
		std::cout << "Adapter: Converting new request to old request." << std::endl;
		legacyObject.oldRequest();
	}
};

// Test the adapter
int main() {
	ModernInterface* adapter = new Adapter();
	adapter->newRequest();

	delete adapter;
	return EXIT_SUCCESS;
}
