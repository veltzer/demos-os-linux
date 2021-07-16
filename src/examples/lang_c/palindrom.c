/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for printf(3), fgets(3)
#include <string.h>	// for strlen(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_NOT_NULL()

int main(int argc, char** argv, char** envp) {
	const int STRINGSIZE=30;
	char mystring[STRINGSIZE];
	printf("Give me a string to check: ");
	CHECK_NOT_NULL(fgets(mystring, STRINGSIZE, stdin));
	int len=strlen(mystring);
	printf("len is %d\n", len);
	int palindrom=1;
	int i;
	for(i=0; i<len; i++) {
		if(mystring[i]!=mystring[len-i-1]) {
			palindrom=0;
			break;
		}
	}
	if(palindrom) {
		printf("YESS!!!!! string IS a palindrom...\n");
	} else {
		printf("bummer, string is not a palindrom...\n");
	}
	return EXIT_SUCCESS;
}
