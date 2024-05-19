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
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>
#include <cpuid.h>

int main() {
	/*
	uint64_t random_number;

	// Check if RDRAND is supported
	if (!__get_cpuid_count(0x07, 0, NULL, NULL, NULL, NULL) & (1 << 30)) {
		printf("RDRAND instruction is not supported on this CPU.\n");
		return 1;
	}

	// Generate a random 64-bit number
	if (_rdrand64_step(&random_number) == 0) {
		printf("Failed to generate a random number.\n");
		return 1;
	}

	printf("Random number: %llu\n", random_number);
	*/
	return EXIT_SUCCESS;
}
