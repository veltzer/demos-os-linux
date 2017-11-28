/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2017 Mark Veltzer <mark.veltzer@gmail.com>
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
 * This example explores the do { ... } while(0) macro construct.
 *
 * Notes:
 * - do-while(0) allows you to break in the middle of a multi-statement macro.
 * see the example below.
 * - if you use do-while(0) correctly and define your macro WITHOUT a semi-colon
 * at the end of the definition then your user will be forced to put a semi-colon
 * him/her self. This is good since this means that the do-while(0) construct
 * behaves like a real function call, which means that if, in the future, you would
 * like to substitute the macro for a real (inline?) function, you could.
 *
 * References:
 * http://stackoverflow.com/questions/1067226/c-multi-line-macro-do-while0-vs-scope-block
 * http://stackoverflow.com/questions/10720465/do-while0-vs-if-1-in-macros
 * http://stackoverflow.com/questions/154136/do-while-and-if-else-statements-in-c-c-macros
 */

#define simple(m) do { printf(m "\n"); } while(0)
#define wrong_macro(m) printf(m " part1\n"); printf(m " part2\n")
#define right_macro(m) do { printf(m " part1\n"); printf(m " part2\n"); } while(0)
#define break_macro(a,m) do { printf(m "\n"); if(a>4) break; printf(m "\n"); } while(0)
#define if_based(m) if(1) { printf(m "\n"); } else {}
#define wrong_if_based(m) if(1) { printf(m "\n"); }

int main(int argc, char** argv, char** envp) {
	// the next line will not compile if not ended with a semi-colon,
	// which is what we want to happen...
	simple("you must end this with a semi-colon");
	// this should not print anything and yet it does...
	if(false)
		wrong_macro("this should be silent, but it is not");
	if(false)
		right_macro("you will never see this, and that is good");
	// example of breaking early out of the macro
	break_macro(1, "this should print twice");
	break_macro(5, "this should print once");
	// the if-else approach works but does not require you to end with a semi-colon...:(
	if_based("if(1) {...} else {} approach works")
	// if-based solution without the else is just wrong
	// gcc with all warnings will prevent this so we ease up on parentheses warnings
	#pragma GCC diagnostic ignored "-Wparentheses"
	#pragma GCC diagnostic ignored "-Wdangling-else"
	if(false)
		wrong_if_based("this should not print")
	else
		printf("this should print, but will not\n");
	return EXIT_SUCCESS;
}
