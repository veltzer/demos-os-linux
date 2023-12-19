/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <map>
using namespace std;

int main(int argc, char** argv, char** envp) {
	map<string, string> myMap;

	// Populate
	myMap["red"] = "rojo";
	myMap["green"] = "verde";
	myMap["blue"] = "azul";

	// Iterate and print
	for(auto it = myMap.begin(); it != myMap.end(); ++it) {
		cout << it->first << " : " << it->second << '\n';
	}
	for(map<string, string>::iterator it = myMap.begin(); it != myMap.end(); ++it) {
		cout << it->first << " : " << it->second << '\n';
	}
	return EXIT_SUCCESS;
}
