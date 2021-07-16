/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <search.h>	// for hsearch(3), ENTRY
#include <stdlib.h>	// for EXIT_SUCCESS, exit(3), EXIT_FAILURE

/*
 * This is a demo of using the GNU C libraries hash table implementation
 */

const char *data[]={
	"alpha", "bravo", "charlie", "delta",
	"echo", "foxtrot", "golf", "hotel", "india", "juliet",
	"kilo", "lima", "mike", "november", "oscar", "papa",
	"quebec", "romeo", "sierra", "tango", "uniform",
	"victor", "whisky", "x-ray", "yankee", "zulu"
};

int main(int argc, char** argv, char** envp) {
	ENTRY e, *ep;

	/* starting with small table, and letting it grow does not work */
	hcreate(30);
	for(int i=0; i<24; i++) {
		e.key=(char*)(data[i]);
		/* data is just an integer, instead of a
		 * pointer to something
		 */
		e.data=(void*)(unsigned long)i;
		ep=hsearch(e, ENTER);
		/* there should be no failures */
		if(ep==NULL) {
			fprintf(stderr, "entry failed\n");
			exit(EXIT_FAILURE);
		}
	}
	for(int i=22; i<26; i++) {
		/* print two entries from the table, and
		 * show that two are not in the table
		 */
		e.key=(char*)(data[i]);
		ep=hsearch(e, FIND);
		printf("%9.9s -> %9.9s:%zd\n",
			e.key,
			ep ? ep->key : "NULL",
			ep ? (ssize_t)(ep->data) : 0
			);
	}
	return EXIT_SUCCESS;
}
