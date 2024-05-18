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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example explores the { ... } macro construct.
 *
 * References:
 * http://stackoverflow.com/questions/1067226/c-multi-line-macro-do-while0-vs-scope-block
 */

#define simple(m) { printf(m "\n"); }

int main() {
	// this is one of the serious problems with a macro in it's
	// own block: it doesn't require a semi-colon after it and thus does
	// not look like a function call. Which means that if, in the future,
	// you will want to turn it into a function call you may need to
	// add semi-colons. This code actually compiles...:)
	simple("you must end this with a semi-colon")
	// this is another example of whats wrong with a block macro: it doesn't
	// behave well in an if-else context. The next example WILL NOT COMPILE
	// if you a seemingly appropriate semi-color after the first call to the
	// macro. This is because the semi-color "closes" the if statement and
	// the following 'else' will seem un-attached to the compiler.
	if(false)
		simple("one")
	else
		simple("two");
	return EXIT_SUCCESS;
}
