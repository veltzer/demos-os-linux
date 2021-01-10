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
#include <search.h>	// for hcreate_r(3), hdestroy_r(3), hsearch_r(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_NOT_ZERO()

/*
 * This is a demo of using the GNU C libraries hash table implementation
 */

// the hash table
static hsearch_data HTAB;

int hash_init(void) {
	int res=hcreate_r(100, &HTAB);
	return res;
}

int hash_destroy( void) {
	hdestroy_r(&HTAB);
	return 1;
}

const char* hash_get(const char* key) {
	ENTRY item;
	item.key=(char*)key;
	ENTRY* ritem;
	int ret=hsearch_r(item, FIND, &ritem, &HTAB);
	if(ret==0) {
		return NULL;
	} else {
		return (const char*)(ritem->data);
	}
}

int hash_put(const char* key, const char* data) {
	ENTRY item;
	item.key=(char*)key;
	item.data=(void*)data;
	ENTRY* ritem;
	return hsearch_r(item, ENTER, &ritem, &HTAB);
}

const char* d1_key="mark";
const char* d1_val="veltzer";

int main(int argc, char** argv, char** envp) {
	CHECK_NOT_ZERO(hash_init());
	printf("after init\n");
	CHECK_NOT_ZERO(hash_put(d1_key, d1_val));
	printf("after put\n");
	const char* getval=hash_get(d1_key);
	printf("got %s\n", getval);
	CHECK_NOT_ZERO(hash_destroy());
	return EXIT_SUCCESS;
}
