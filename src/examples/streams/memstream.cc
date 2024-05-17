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
#include <stdlib.h>	// for EXIT_SUCCESS, free(3)
#include <stdio.h>	// for open_memstream(3), fclose(3), fwrite(3)
#include <err_utils.h>	// for CHECK_NOT_NULL_FILEP(), CHECK_ZERO_ERRNO(3)

/*
 * This example demonstrates the use of a memstream which is a FILE* which writes
 * to a chunk of RAM.
 *
 * NOTES:
 * - the buffer that holds the stream data is fully managed by the memory stream.
 * - you do not need to allocate it and it grows as needed.
 * - the only thing you do need to do is to free(3) it after closing the stream.
 */

int main() {
	char* ptr;
	size_t sizeloc;
	FILE* fp=CHECK_NOT_NULL_FILEP(open_memstream(&ptr, &sizeloc));
	const char* to_print="Hello, World!\n";
	CHECK_INT_NOERRNO(fwrite(to_print, strlen(to_print), 1, fp), 1);
	CHECK_ZERO_ERRNO(fclose(fp));
	printf("ptr is now [%s]\n", ptr);
	printf("sizeloc is now [%zd]\n", sizeloc);
	// we must free the pointer outselves! (free(3) does not return a success code)
	free(ptr);
	return EXIT_SUCCESS;
}
