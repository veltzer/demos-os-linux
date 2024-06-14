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
#include <vector>

using namespace std;

class AlarmListener { public: virtual void alarm() = 0; };

class SensorSystem {
	vector<AlarmListener*> listeners;
public:
	void attach( AlarmListener* al ) { listeners.push_back( al ); }
	void soundTheAlarm() {
		for (unsigned int i=0; i < listeners.size(); i++)
			listeners[i]->alarm();
} };
/////////////////////////////////////////////////////////////////////

class Lighting : public AlarmListener {
	public: /*virtual*/ void alarm() { cout << "lights up" << '\n'; }
};

class Gates : public AlarmListener {
	public: /*virtual*/ void alarm() { cout << "gates close" << '\n'; }
};

class CheckList {
	virtual void localize() { cout << "	establish a perimeter" << '\n'; }
	virtual void isolate() { cout << "	isolate the grid"		<< '\n'; }
	virtual void identify() { cout << "	identify the source"	<< '\n'; }
public:
	void byTheNumbers() { // Template Method design pattern
		localize();
		isolate();
		identify();
} };
							// class inheri. // type inheritance
class Surveillance : public CheckList, public AlarmListener {
	/*virtual*/ void isolate() { cout << "	train the cameras" << '\n'; }
public:
	/*virtual*/ void alarm() {
		cout << "Surveillance - by the numbers:" << '\n';
		byTheNumbers();
} };

int main() {
	SensorSystem ss;
	ss.attach(new Gates());
	ss.attach(new Lighting());
	ss.attach(new Surveillance());
	ss.soundTheAlarm();
	return 0;
}

