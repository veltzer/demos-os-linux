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
#include <ace/ARGV.h>
#include <ace/Get_Opt.h>
#include <ace/ARGV.h>
#include <iostream>	// for std::cout, std::endl
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * usage example: Time_Value_Class -e 36000 -i 1200
 * Where: 1200 means 20 minutes
 * 36000 means 10 hours
 *
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	const ACE_Time_Value max_interval(60*60);	// 1 hour.
	// ACE_Time_Value expiration=ACE_OS::gettimeofday();
	ACE_Time_Value expiration;
	ACE_Time_Value interval;
	ACE_Time_Value current_time=expiration;

	ACE_Get_Opt opt(argc, argv, "e:i:");
	for(int c; (c=opt())!=-1; ) {
		switch (c) {
		case 'e':
			// Arithmetic operation
			expiration+=ACE_Time_Value(atoi(opt.opt_arg()));
			break;

		case 'i':
			// Assignment
			interval=ACE_Time_Value(atoi(opt.opt_arg()));
			break;
		}
	}
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("interval=%d max_interval=%d\n"), interval.sec(), max_interval.sec()));
	if (interval > max_interval) {
		std::cout << "interval must be less than " << max_interval.sec() << std::endl;
	} else if (expiration > (ACE_Time_Value::max_time - interval)) {
		std::cout << "expiration + interval must be less than " << ACE_Time_Value::max_time.sec() << std::endl;
	}
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("interval: %d seconds.\n"), interval.sec()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("expiration after: %d seconds.\n"), expiration.sec() - current_time.sec()));
	return EXIT_SUCCESS;
}
