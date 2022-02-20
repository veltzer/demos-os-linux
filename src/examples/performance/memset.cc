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

#include <firstinclude.h>
#include <string.h>	// for memset(3)
#include <stdlib.h>	// for malloc(3)
#include <wchar.h>	// for wmemset(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <disassembly_utils.h>	// for disassemble_main()

/*
 * This example explores the inner workings of memset...
 *
 * From the disassembly we can see that:
 * - no call to memset is used since memset is a macro.
 *	(you cannot break on memset in the debugger and actually stop on that function)
 * - this means that memset inline assembly is inserted into our code.
 * - the compiler uses "rep stos" to do the actual memsetting which is
 *	a single machine instruction on Intel which means that it is very efficient.
 * - the wmemset function is not inline assembly (you can break on it in the debugger)
 * - this example does not explore the inner workins of the wmemset function
 * (meanins whether it is implemented efficiently or not).
 *
 * this is to make sure that source code interleaving in the disassembly works
 * out well...
 * EXTRA_COMPILE_FLAGS=-g3
 *
 * TODO:
 * - find out how wmemset works and discuss it's efficiency.
 */

int main(int argc, char** argv, char** envp) {
	const int size=1000;
	char* buf=(char*)malloc(size);
	memset(buf, 0, 8);
	memset(buf, 0, size);
	for(int i=0; i<size; i++) {
		buf[i]=0;
	}
	wchar_t* buf2=(wchar_t*)malloc(size*sizeof(wchar_t));
	wmemset(buf2, 0, size);
	disassemble_main();
	return EXIT_SUCCESS;
}
