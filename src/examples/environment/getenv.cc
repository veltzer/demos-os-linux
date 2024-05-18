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
#include <iostream>	// for std::cout, std::endl
#include <stdlib.h>	// for EXIT_SUCCESS, getenv(3)
#include <string.h>	// for strcmp(3)

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
		std::cout << *envp << std::endl;
		envp++;
	}
	// print it again via environ
	extern char **environ;
	char** environp=environ;
	while(*environp!=NULL) {
		std::cout << *environp << std::endl;
		environp++;
	}
	const char* varname="WHOWANTSSOUP";
	char* ptr=getenv(varname);
	if(ptr==NULL) {
		std::cout << "Please stand in line (define environment variable WHOWANTSSOUP)" << std::endl;
		std::cout << "in bash use [export WHOWANTSSOUP=kramer]" << std::endl;
	} else {
		if(strcmp(ptr, "kramer")==0) {
			std::cout << "Jumbalaya!" << std::endl;
		} else {
			if(strcmp(ptr, "newman")==0) {
				std::cout << "No soup for you!" << std::endl;
			} else {
				std::cout << "You are not kramer or newman" << std::endl;
				std::cout << "in bash use [export WHOWANTSSOUP=kramer|newman]" << std::endl;
			}
		}
	}
	return EXIT_SUCCESS;
}
