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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <iostream>	// for cerr, endl
#include <string>	// for string
#include <exception>	// for exception

using namespace std;

/*
 * This is an example of using custom exception types...
 */

class MyException: public exception {
private:
	string message;

public:
	MyException(const char* imessage) {
		message = imessage;
	}
	virtual const char* operator()() {
		cout << message << endl;
		return message.c_str();
	}
	virtual const char* what() const throw(){
		cout << message << endl;
		return message.c_str();
	}
	virtual~MyException() throw(){
	};
};

int main() {
	try {
		throw MyException("this is a message");
	} catch(const MyException& e) {
		cerr << "got the exception " << e.what() << endl;
	}
	return EXIT_SUCCESS;
}
