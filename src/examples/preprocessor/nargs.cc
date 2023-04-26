/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <preprocessor_utils.h>	// for PP_NARG

#define PRINT(pp_narg) printf("%2d = %s\n", pp_narg, # pp_narg)

int main(int argc, char** argv, char** envp) {
	PRINT(PP_NARG(A));
	PRINT(PP_NARG(A, B));
	PRINT(PP_NARG(A, B, C));
	PRINT(PP_NARG(A, B, C, D));
	PRINT(PP_NARG(A, B, C, D, E));

	PRINT(PP_NARG(1, 2, 3, 4, 5, 6, 7, 8, 9, 0, // 1..10
		1, 2, 3, 4, 5, 6, 7, 8, 9, 0, // 11..20
		1, 2, 3, 4, 5, 6, 7, 8, 9, 0, // 21..30
		1, 2, 3, 4, 5, 6, 7, 8, 9, 0, // 31..40
		1, 2, 3, 4, 5, 6, 7, 8, 9, 0, // 41..50
		1, 2, 3, 4, 5, 6, 7, 8, 9, 0, // 51..60
		1, 2, 3));

	/**
	** If the number of arguments to PP_NARG() is greater than 63, the
	** 64th argument is returned. This is well-defined behaviour, but
	** not exactly what was intended.
	*/
	PRINT(PP_NARG(1, 2, 3, 4, 5, 6, 7, 8, 9, 0, // 1..10
		1, 2, 3, 4, 5, 6, 7, 8, 9, 0, // 11..20
		1, 2, 3, 4, 5, 6, 7, 8, 9, 0, // 21..30
		1, 2, 3, 4, 5, 6, 7, 8, 9, 0, // 31..40
		1, 2, 3, 4, 5, 6, 7, 8, 9, 0, // 41..50
		1, 2, 3, 4, 5, 6, 7, 8, 9, 0, // 51..60
		1, 2, 3, -123456789));

	PRINT(PP_NARG(1, 2, 3, 4, 5, 6, 7, 8, 9, 0, // 1..10
		1, 2, 3, 4, 5, 6, 7, 8, 9, 0, // 11..20
		1, 2, 3, 4, 5, 6, 7, 8, 9, 0, // 21..30
		1, 2, 3, 4, 5, 6, 7, 8, 9, 0, // 31..40
		1, 2, 3, 4, 5, 6, 7, 8, 9, 0, // 41..50
		1, 2, 3, 4, 5, 6, 7, 8, 9, 0, // 51..60
		1, 2, 3, -123456789, -987654321));

	return EXIT_SUCCESS;
}
