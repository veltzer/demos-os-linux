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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <us_helper.h>	// for stringify()

/*
 * This example tells you on what type of endian machine you are.
 * This relies on gcc which defines __BYTE__ORDER__.
 * You can see other gcc native defines by using:
 * gcc -E -dM - < /dev/null
 *
 * References:
 * http://stackoverflow.com/questions/2100331/c-macro-definition-to-determine-big-endian-or-little-endian-machine
 */

int main(int argc, char** argv, char** envp) {
	printf("__BYTE_ORDER__ is [%s]\n", stringify(__BYTE_ORDER__));
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	printf("your system is __ORDER_LITTLE_ENDIAN__\n");
#elif __BYTE_ORDER__ == __ORDER__BIG_ENDIAN__
	printf("your system is __ORDER_BIG_ENDIAN__\n");
#else
#error "This is a weird system you have"
#endif
	return EXIT_SUCCESS;
}
