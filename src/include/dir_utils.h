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

#ifndef __dir_utils_h
#define __dir_utils_h

/*
 * This is a collection of functions to help deal with directory
 * entries (dirent) on Linux.
 */

/* THIS IS A C FILE, NO C++ here */

#include <firstinclude.h>
#include <us_helper.h>	// for ARRAY_SIZEOF()
#include <stdio.h>	// for printf(3)
#include <string.h>	// for strcmp(3), strsignal(3)
#include <signal.h>	// for siginfo_t, sighandler_t, sigaction(2), signal(2)
#include <err_utils.h>	// for CHECK_ASSERT()

/*
 * List of all directory types
 */

typedef struct _dir_val_and_name {
	int val;
	const char* name;
} dir_val_and_name;

/*
 * List of all folder types and names
 */
#define entry(x) { x, # x }
static dir_val_and_name dir_tbl[]={
	entry(DT_REG),
	entry(DT_DIR),
	entry(DT_FIFO),
	entry(DT_SOCK),
	entry(DT_LNK),
	entry(DT_BLK),
	entry(DT_CHR),
};
#undef entry

/*
 * Translate file type name to value
 */
static inline int dir_get_by_name(const char* name) {
	unsigned int i;
	for(i=0; i<ARRAY_SIZEOF(dir_tbl); i++) {
		if(strcmp(name, dir_tbl[i].name)==0) {
			return dir_tbl[i].val;
		}
	}
	CHECK_ERROR("bad file type name");
}

/*
 * Translate file val to name
 */
static inline const char* dir_get_by_val(int val) {
	unsigned int i;
	for(i=0; i<ARRAY_SIZEOF(dir_tbl); i++) {
		if(dir_tbl[i].val==val) {
			return dir_tbl[i].name;
		}
	}
	CHECK_ERROR("unknown");
}

/*
 * Print out a table of all file type values, names and descriptions
 */
static inline void print_dir_table() {
	unsigned int i;
	printf("number of dir values is %zd\n", ARRAY_SIZEOF(dir_tbl));
	for(i=0; i<ARRAY_SIZEOF(dir_tbl); i++) {
		int val=dir_tbl[i].val;
		const char* name=dir_tbl[i].name;
		printf("i=%d, val=%d, name=%s\n", i, val, name);
	}
}

#endif	/* !__dir_utils_h */
