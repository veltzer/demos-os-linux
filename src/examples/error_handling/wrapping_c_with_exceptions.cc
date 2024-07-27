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
#include <exception>	// for exception
#include <iostream>	// for cerr
#include <signal.h>	// for signal(2)
#include <unistd.h>	// for pipe(2), syscall(2)
#include <sys/syscall.h>// for syscall(2)
#include <cstdlib>

using namespace std;

/*
 * This example shows how to eliminate lots of redundant C error checking
 * from your code when calling system calls or C APIs from C++ and turning
 * all of those errors into exceptions...
 *
 * Why is this good ? Explain the theory behind exceptions.
 */

// this is the non template approach
int syscall(int val, int err_val) {
	if(val==err_val) {
		throw exception();
	}
	return val;
}

// Here is a template that will take care of all our needs
template<class T> T syscall(T val, T err_val) {
	if(val==err_val) {
		throw exception();
	}
	return val;
}

void myhandler(int sig) {
	cerr << "in signal handler for signal " << sig << endl;
}

int main() {
	// here is an example of using this construct
	int fd[2];
	try {
		syscall(pipe(fd), -1);
		syscall(signal(SIGPIPE, myhandler), SIG_ERR);
	} catch (exception& e) {
		cerr << "cought exception" << endl;
	}
	return EXIT_SUCCESS;
}
