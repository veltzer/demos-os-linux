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
#include <cstdlib>
#include <cstring>

using namespace std;

/*
 * A simple example for using environment variables...
 * This example also shows how to receive environment variables
 * via the "char** envp" argument to main and how to print it.
 * It also shows how to access all environment variables via
 * the 'environ' extern variable.
 */

int main(int argc __attribute__((unused)), char** argv __attribute__((unused)), char** envp) {
	// print out the entire environment
	while(*envp!=NULL) {
		cout << *envp << endl;
		envp++;
	}
	// print it again via environ
	extern char **environ;
	char** environp=environ;
	while(*environp!=NULL) {
		cout << *environp << endl;
		environp++;
	}
	const char* varname="WHOWANTSSOUP";
	char* ptr=getenv(varname);
	if(ptr==NULL) {
		cout << "Please stand in line (define environment variable WHOWANTSSOUP)" << endl;
		cout << "in bash use [export WHOWANTSSOUP=kramer]" << endl;
	} else {
		if(strcmp(ptr, "kramer")==0) {
			cout << "Jumbalaya!" << endl;
		} else {
			if(strcmp(ptr, "newman")==0) {
				cout << "No soup for you!" << endl;
			} else {
				cout << "You are not kramer or newman" << endl;
				cout << "in bash use [export WHOWANTSSOUP=kramer|newman]" << endl;
			}
		}
	}
	return EXIT_SUCCESS;
}
