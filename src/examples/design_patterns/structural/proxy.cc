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
#include <string>

using namespace std;


class RealImage {
	int id;
public:
	RealImage( int i ) { id = i; cout << "	$$ ctor: "<< id << '\n'; }
	~RealImage()					 { cout << "	dtor: " << id << '\n'; }
	void draw()					{ cout << "	drawing image " << id << '\n'; }
};

// 1. Design an "extra level of indirection" wrapper class
class Image {
	// 2. The wrapper class holds a pointer to the real class
	RealImage* theRealThing;
	int		id;
	static int next;
public:
	Image() { id = next++; theRealThing = 0; } // 3. Initialized to null
	~Image() { delete theRealThing; }
	void draw() {
		// 4. When a request comes in, the real object is created "on first use"
		if ( ! theRealThing) theRealThing = new RealImage( id );
		// 5. The request is always delegated
		theRealThing->draw();
	}
};
int Image::next = 1;

int main() {
	Image images[5];
	int	i;

	cout << "Exit[0], Image[1-5]: ";
	cin >> i;
	while (i) {
		images[i-1].draw();
		cout << "Exit[0], Image[1-5]: ";
		cin >> i;
	}
	return 0;
}

