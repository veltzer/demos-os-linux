/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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
#include "compiler_builtins.hh"	// for print_from_header()

/*
 * This example explores compiler builtins like:
 * __FILE__, __LINE__, __func__, __FUNCTION__, __PRETTY_FUNCTION__, __BASE_FILE__
 * Which can be used in order to produce better error/logging/debug/warning messages.
 *
 * Notice that if you put the prints, even in an inline function, then you get the data
 * for the ** inline function ** which is the calee and not for the caller. This is
 * probably what you want. A way to change this behaviour is to use a macro
 * (also demonstrated below).
 *
 * Notice that __FILE__ and __LINE__ are different from __func__, __FUNCTION__ and
 * __PRETTY_FUNCTION__ in that the former are supplied by the pre processor and the latter
 * are supplied by the compiler. This distinction however is not very important.
 *
 * A frequently asked question here is how to indent the printing to match the level
 * of stack at which the printing occurs. This is not very easy as there is no variable
 * "stacklevel" or some such which indicated the stack level and can facilitate this.
 * However, you can use function entry and exit handlers, either explicit or implicit,
 * to facilitate this.
 *
 * Another important issue is the difference between __BASE_FILE__ and __FILE__. Usually
 * you want __FILE__ since __BASE_FILE__ is not where you are but rather the main file
 * which you are compiling.
 */

class A {
public:
	void thisMethod(int a, int b) {
		PRINT_BUILTINS();
	}
};

int main(int argc, char** argv, char** envp) {
	printf("Here is a print using the macro:\n");
	PRINT_BUILTINS();
	printf("Here is a print using the inline function:\n");
	print_builtins();
	printf("Here is a print from the header file:\n");
	print_from_header();
	A a;
	a.thisMethod(5, 6);
	return EXIT_SUCCESS;
}
