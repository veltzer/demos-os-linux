/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <iostream>	// for std::cout, std::endl
#include <map>	// for std::map<T1,T2>, std::map<T1,T2>::iterator
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * An example of using an STL map.
 *
 * References:
 * http://www.yolinux.com/TUTORIALS/CppStlMultiMap.html
 */

int main(int argc, char** argv, char** envp) {
	std::map<int, int> m;
	m[6]=7;
	m[5]=3;
	m[4]=-14;
	// lets print the size of the map
	std::cout << "size of the map is " << m.size() << std::endl;
	if(m.find(6)!=m.end()) {
		std::cout << "6 is in the map" << std::endl;
		std::cout << "its partner is " << m.find(6)->second << std::endl;
	}
	for(std::map<int, int>::iterator ii=m.begin(); ii!=m.end(); ++ii) {
		std::cout << (*ii).first << ": " << (*ii).second << std::endl;
	}
	m.erase(5);
	for(std::map<int, int>::iterator ii=m.begin(); ii!=m.end(); ++ii) {
		std::cout << (*ii).first << ": " << (*ii).second << std::endl;
	}
	return EXIT_SUCCESS;
}
