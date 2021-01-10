/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This is an example of how C initializes array of characters.
 * The whole of point of this example is to show that C does not
 * treat initialization of arrays of characters from string
 * literals as initializing strings but rather as initializing
 * arrays. Using a string literal to initialize an array of
 * characters is just convenience as opposed to specifying each
 * individual characters seperately. All the characters in the
 * array which are not explictly initialized will be initialized
 * to 0 (according to the specfication). This goes well with
 * using the content of that array as string, IF, AND ONLY IF,
 * you initialize with less content than the size of the array.
 * But the compiler doesn't force you to do that.
 *
 * Notes:
 * - intializing an array with MORE characters than space in the array
 * WILL create a compile time error.
 */

int main(int argc, char** argv, char** envp) {
	// this is NOT an error since this is just array initialization
	// (not 0 at the end)
	char hello[5] __attribute__((unused))="hello";
	// this will intialize all 50-3 characters left in the array to 0
	char foo[50] __attribute__((unused))="foo";
	// so the above is the same as:
	char bar[50] __attribute__((unused))={ 'f', 'o', 'o' };
	// by now it should be fairly obvious why the following is legal...
	struct bullshit {
		char str[4];
		unsigned int num;
	} test __attribute__((unused))= {"stop", 0xFFFF};	/* 's' 't' 'o' 'p' 0xF */
	return EXIT_SUCCESS;
}
