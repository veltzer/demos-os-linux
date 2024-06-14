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
#include <ctime>

using namespace std;

class Link {
	int	id;
	Link* next;							// 1. "next" pointer
	static int	theBid;				// 4. Current bid and bidder
	static Link* bidder;
public:
	Link( int num ) { id = num; next = 0; }
	void addLast( Link* l ) {
		if (next) next->addLast( l ); // 2. Handle and/or pass on
		else		next = l;
	}
	void bid() {
		int num = rand() % 9;
		cout << id << '-' << num << " ";
		if (num < theBid) {
			theBid = num;					// 4. Current bid and bidder
			bidder = this;
		}
		if (next) next->bid();			// 2. Handle and/or pass on
		else		bidder->execute();	// 5. The last 1 assigns the job
	}
	void execute() {
		cout << id << " is executing\n";
		theBid = 999;
}};
int	Link::theBid = 999;				// 4. Current bid and bidder
Link* Link::bidder = 0;

int main() {
	Link chain( 1 );
	for(int i=2; i < 7; i++)
		chain.addLast( new Link( i ) );
	srand( time( 0 ) );
	for(int i=0; i < 10; i++)
		chain.bid();						 // 3. Client "launches & leaves"
	return 0;
}
