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
#include <stdlib.h>	// for EXIT_SUCCESS
#include <stdio.h>	// for printf(3)
#include <time.h>	// for tm:struct, time_t:type, mktime(3)
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * This example shows that mktime(3) cannot cope with dates that it does not
 * have time zone info for. It returns errors for these dates.
 *
 * If you want to see up to which time you have time zone info tables installed
 * on your system use:
 * zdump -v Asia/Jerusalem
 *
 * TODO:
 * - find out how to programatically know up to which date I have time zone info
 * installed on the system.
 */

int main(int argc, char** argv, char** envp) {
	for(unsigned int i=0; i<50; i++) {
		int year=2000+i;
		struct tm mytime;
		mytime.tm_sec=0;
		mytime.tm_min=0;
		mytime.tm_hour=0;
		mytime.tm_mday=30;
		mytime.tm_mon=4;
		mytime.tm_year=year-1900;
		mytime.tm_isdst=0;

		time_t time1=CHECK_NOT_M1(mktime(&mytime));
		mytime.tm_isdst=1;
		time_t time2=CHECK_NOT_M1(mktime(&mytime));
		mytime.tm_isdst=-1;
		time_t time3=CHECK_NOT_M1(mktime(&mytime));
		printf("year is %d, time1 is %ld, time2 is %ld, time3 is %ld\n", year, time1, time2, time3);
	}
	return EXIT_SUCCESS;
}
