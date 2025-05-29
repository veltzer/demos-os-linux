/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for exit(3), posix_memalign(3), EXIT_SUCCESS
#include <unistd.h>	// for getpagesize(2)
#include <sys/mman.h>	// for mprotect(2)
#include <err_utils.h>	// for CHECK_NOT_M1()

// this code simulates a memory corruption
void bad_code(void* precious_data) {
	char* p=(char*)precious_data;
	p[0]=5;
}

void protect_me(void* precious_data, size_t len) {
	CHECK_NOT_M1(mprotect(precious_data, len, PROT_READ));
}

int main() {
	void* precious_data=NULL;
	const unsigned int size=8192;
	CHECK_ZERO(posix_memalign(&precious_data, getpagesize(), size));
	protect_me(precious_data, size);
	bad_code(precious_data);
	return EXIT_SUCCESS;
}
