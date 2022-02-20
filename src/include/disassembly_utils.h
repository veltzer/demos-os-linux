/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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

#ifndef __disassembly_utils
#define __disassembly_utils

/*
 * This file provides helper function for disassembly.
 */

#include <firstinclude.h>
#include <us_helper.h>	// for get_program_name()
#include <multiproc_utils.h>	// for my_system()

/*
 * This funcation will disassemble the executable in which it is run
 * Make sure to compile the code with -g if you want to see the interleaved
 * source code.
 */
static inline void disassemble_me() {
	char myname[1024];
	get_program_name(myname, sizeof(myname));
	my_system("objdump --disassemble --source %s", myname);
	// my_system("objdump --disassemble --source %s --start-address main", argv[0]);
}

/*
 * Just disassemble the 'main' function.
 * This uses gdb to do the work.
 */
static inline void disassemble_main() {
	char myname[1024];
	get_program_name(myname, sizeof(myname));
	my_system("gdb --batch -ex \"disassemble /m main\" %s", myname);
}

/*
 * disassemble a single function.
 * The function code must be in the current executables physical file.
 * This means it cannot be in a DSO.
 * If you want to disassemble a function from a DSO, link you application
 * statically.
 */
static inline void disassemble_function(const char* function_name) {
	char myname[1024];
	get_program_name(myname, sizeof(myname));
	// my_system("objdump --disassemble-all %s | grep -A%d \\<%s\\>:", myname, lines, name);
	// my_system("objdump --disassemble --source %s | grep -A%d \\<%s\\>:", myname, lines, name);
	// we use sed because we want to grep between the function we look for and
	// the next function
	my_system("objdump --disassemble --source %s | sed -n '/<.*%s.*>:/,/<.*>:/p\'", myname, function_name);
}

#endif	/* !__disassembly_utils */
