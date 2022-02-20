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

/*
 * This example shows how to use C99 initializers to initialize
 * a 'struct tm' correctly. Compare this to the subtle bug on
 * the following line which initializes the tm_idst field in
 * the wrong place. This is because struct tm has MORE fields
 * than are documented... This is actually a bug I saw happening
 * no a real system.
 */

#include <firstinclude.h>
#include <time.h>	// for struct tm
#include <stdlib.h>	// for EXIT_SUCCESS

int main(int argc, char** argv, char** envp) {
	struct tm tempTm __attribute__((unused))={
		.tm_sec=0,
		.tm_min=0,
		.tm_hour=0,
		.tm_mday=0,
		.tm_mon=0,
		.tm_year=0,
		.tm_wday=0,
		.tm_yday=0,
		.tm_isdst=0,
	};
	struct tm tempTm2 __attribute__((unused))={ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	return EXIT_SUCCESS;
}
