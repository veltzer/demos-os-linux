/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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

using namespace std;

class Machine {
	class State* current;
public:
	Machine();
	void setCurrent( State* s ) { current = s; }
	void on();
	void off();
};

class State {
public:
	virtual void on(Machine*) { cout << "	already ON\n"; }
	virtual void off(Machine*) { cout << "	already OFF\n"; }
	virtual ~State() {}
};

void Machine::on() { current->on( this ); }
void Machine::off() { current->off( this ); }

class ON : public State {
public:
	ON() { cout << "	ON-ctor "; };
	~ON() { cout << "	dtor-ON\n"; };
	void off( Machine* m );
};

class OFF : public State {
public:
	OFF() { cout << "	OFF-ctor "; };
	~OFF() { cout << "	dtor-OFF\n"; };
	void on( Machine* m ) {
		cout << "	going from OFF to ON";
		m->setCurrent( new ON() );
		delete this;
	}
};

void ON::off( Machine* m ) {
	cout << "	going from ON to OFF";
	m->setCurrent( new OFF() );
	delete this;
}

Machine::Machine() { current = new OFF(); cout << '\n'; }

int main() {
	void (Machine::*ptrs[])() = { &Machine::off, &Machine::on };
	Machine fsm;
	int num;
	while (1) {
		cout << "Enter 0/1: ";
		cin >> num;
		(fsm.*ptrs[num])();
	}
	return 0;
}

