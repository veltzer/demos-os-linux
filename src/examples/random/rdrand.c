/*
 * This file is part of the demos-os-linux package.
 * Copyright (C) 2011-2026 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-os-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-os-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-os-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <stdio.h>
#include <immintrin.h>

/*
 * This example shows how to use the gcc _rdrand64_step intrinsic function
 * EXTRA_COMPILE_FLAGS_BEFORE=-m64 -mrdrnd
 *
 * References:
 * - https://stackoverflow.com/questions/31214457/how-to-use-rdrand-intrinsics
 */

int main() {
	unsigned long long result;
	int rc = _rdrand64_step(&result);
	printf("%i %llu\n", rc, result);
	return (rc != 1);
}
