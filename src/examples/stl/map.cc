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
#include <iostream>	// for cout, endl
#include <map>	// for map<T1,T2>, map<T1,T2>::iterator
#include <cstdlib>	// for EXIT_SUCCESS

using namespace std;

/*
 * An example of using an STL map.
 *
 * References:
 * http://www.yolinux.com/TUTORIALS/CppStlMultiMap.html
 */

int main() {
	map<int, int> m;
	m[6]=7;
	m[5]=3;
	m[4]=-14;
	// lets print the size of the map
	cout << "size of the map is " << m.size() << endl;
	if(m.find(6)!=m.end()) {
		cout << "6 is in the map" << endl;
		cout << "its partner is " << m.find(6)->second << endl;
	}
	for(map<int, int>::iterator i=m.begin(); i!=m.end(); i++) {
		cout << i->first << ": " << i->second << endl;
	}
	m.erase(5);
	for(map<int, int>::iterator i=m.begin(); i!=m.end(); i++) {
		cout << i->first << ": " << i->second << endl;
	}
	return EXIT_SUCCESS;
}
