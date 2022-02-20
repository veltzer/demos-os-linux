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
#include <stdio.h>	// for printf(3), stdout:object, fflush(3)
#include <stdlib.h>	// for malloc(3), free(3)
#include <err_utils.h>	// for CHECK_NOT_EOF(), CHECK_NOT_NULL()

/*
 * This example explores what is the largest area that can be allocated
 * using malloc(3).
 *
 * The results are that on 64 bit systems the limit seems to be around 17GB.
 * On 32 bit systems it should be around 2GB.
 */

int main(int argc, char** argv, char** envp) {
	printf("sizeof(pointer)=%ld (should be 8 on 64 bit and 4 on 32 bit)\n", sizeof(NULL));
	printf("sizeof(size_t)=%ld (should be 8 on 64 bit and 4 on 32 bit)\n", sizeof(size_t));
	size_t size_to_alloc=1;
	while(true) {
		printf("attempting size %ld...", size_to_alloc);
		CHECK_NOT_EOF(fflush(stdout));
		void* p=CHECK_NOT_NULL(malloc(size_to_alloc));
		printf("OK\n");
		free(p);
		size_to_alloc+=size_to_alloc;
	}
	return EXIT_SUCCESS;
}
