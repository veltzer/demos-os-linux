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

using namespace std;

class A;
class B;
class C;

class FSMstate {
	public:
		virtual void on() { cout << "undefined combo" << endl; }
		virtual void off() { cout << "undefined combo" << endl; }
		virtual void ack() { cout << "undefined combo" << endl; }
};

class FSM {
public:
	FSM();
	void on() { states[current]->on(); current = next[current][0]; }
	void off() { states[current]->off(); current = next[current][1]; }
	void ack() { states[current]->ack(); current = next[current][2]; }
private:
	FSMstate* states[3];
	int current;
	int next[3][3];
	static A a;
	static B b;
	static C c;
};

class A : public FSMstate { public:
	void on() { cout << "A, on ==> A" << endl; }
	void off() { cout << "A, off ==> B" << endl; }
	void ack() { cout << "A, ack ==> C" << endl; }
};
class B : public FSMstate { public:
	void off() { cout << "B, off ==> A" << endl; }
	void ack() { cout << "B, ack ==> C" << endl; }
};
class C : public FSMstate { public:
	void ack() { cout << "C, ack ==> B" << endl; }
};

FSM::FSM() {
	states[0] = &a; states[1] = &b; states[2] = &c;
	current = 1;
	next[0][0] = 0; next[0][1] = 1; next[0][2] = 2;
	next[1][0] = 1; next[1][1] = 0; next[1][2] = 2;
	next[2][0] = 2; next[2][1] = 2; next[2][2] = 1;
}

A FSM::a;
B FSM::b;
C FSM::c;


enum	Message { On, Off, Ack };
Message messageArray[10] = { On,Off,Off,Ack,Ack,Ack,Ack,On,Off,Off };

int main() {
	FSM fsm;
	for (int i = 0; i < 10; i++) {
		if (messageArray[i] == On) fsm.on();
		else if (messageArray[i] == Off) fsm.off();
		else if (messageArray[i] == Ack) fsm.ack();
	}
	return 0;
}

