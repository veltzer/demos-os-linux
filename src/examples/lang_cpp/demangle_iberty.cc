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
#define HAVE_DECL_BASENAME 1
#include <libiberty/demangle.h>	// for cplus_demangle(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <stdio.h>	// for printf(3)
#include <us_helper.h>	// for ARRAY_SIZEOF()

/*
 * Demangle c++ symbols using libiberty (which is -liberty when linking on
 * the command line...)
 *
 * NOTES:
 * - This is not the only API for demangling. There is a abi::__cxa_demangle for this too.
 * - From this example it seems tha the right flag to use for demangling is 'DMGL_PARAMS'.
 * - the HAVE_DECL_BASENAME definition above is critical if you want to compile...:)
 * - it does not seem that pkg-config knows about libiberty so we have to use explicit
 * command line flags to link with it.
 *
 * EXTRA_LINK_FLAGS_AFTER=-liberty
 */

typedef struct _dmgl_val_and_name {
	int val;
	const char* name;
} dmgl_val_and_name;

#define entry(x) { x, # x }
static dmgl_val_and_name dmgl_tbl[]={
	entry(DMGL_NO_OPTS),
	//entry(DMGL_PARAMS),
	entry(DMGL_ANSI),
	entry(DMGL_JAVA),
	entry(DMGL_VERBOSE),
	entry(DMGL_TYPES),
	entry(DMGL_RET_POSTFIX),
	entry(DMGL_RET_DROP),
	entry(DMGL_AUTO),
	//entry(DMGL_GNU),
	//entry(DMGL_LUCID),
	//entry(DMGL_ARM),
	//entry(DMGL_HP),
	//entry(DMGL_EDG),
	entry(DMGL_GNU_V3),
	entry(DMGL_GNAT),
	entry(DMGL_STYLE_MASK),
};

int main(int argc, char** argv, char** envp) {
	const char* mangled="_ZN1A3addEii";
	for(unsigned int i=0;i<ARRAY_SIZEOF(dmgl_tbl); i++) {
		const int val=dmgl_tbl[i].val;
		const char* name=dmgl_tbl[i].name;
		char* s=cplus_demangle(mangled, val);
		printf("mangled [%s] and demangled is [%s] ,name is [%s]\n", mangled, s, name);
	}
	return EXIT_SUCCESS;
}
