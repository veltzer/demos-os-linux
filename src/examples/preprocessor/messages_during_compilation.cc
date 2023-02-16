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
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example shows how to emit messages during compilation
 *
 * References:
 * https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html
 * https://html.developreference.com/article/18627906/How+to+hide+extra+output+from+%23pragma+message
 *
 * TODO:
 * - I cant seem to print the function name from #pragma message
 *
 * EXTRA_COMPILE_FLAGS=-fno-diagnostics-show-caret -ftrack-macro-expansion=0 -fno-diagnostics-show-option
 * ALLOW_OUTPUT
 */

#define quote(a) # a
#define stringify(x) quote(x)

#pragma message "Compiling " __FILE__ " in line " stringify(__LINE__)

// another way to do any kind of pragma
_Pragma("message \"foo\"")

#define DO_PRAGMA(x) _Pragma (#x)
#define TODO(x) DO_PRAGMA(message ("TODO - " #x))

TODO(Remember to fix this)

int main(int argc, char** argv, char** envp) {
#pragma message "Compiling " __FILE__ " in line " stringify(__LINE__) " in function " quote(__FUNCTION__)
	return EXIT_SUCCESS;
}
