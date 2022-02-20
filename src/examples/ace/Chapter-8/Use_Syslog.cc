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
#include <ace/Log_Msg.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * Notes:
 * - note the care which is taken to keep the compiler from
 * inlining the foo function.
 *
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 */

void __attribute__((noinline)) foo(void) {
	ACE_TRACE("foo");
	ACE_DEBUG((LM_INFO, ACE_TEXT("%IThis is a log test\n")));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("%I(0)Is this info seen?\n")));
	ACE_DEBUG((LM_INFO, ACE_TEXT("%I(1)Is this info seen?\n")));
}

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	// This will be directed to stderr (the default ACE_Log_Msg behavior).
	ACE_TRACE("main");
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("%IHi Mom\n")));
	// Everything from foo() will be directed to the system logger
	// open: Program Name
	// ACE_Log_Msg::SYSLOG - Use system log file
	// text that will appear on each logged line
	ACE_LOG_MSG->open(argv[0], ACE_Log_Msg::SYSLOG, ACE_TEXT("syslogTest"));
	foo();
	// Now we reset the log output to default (stderr)
	ACE_LOG_MSG->open(argv[0]);
	ACE_DEBUG((LM_INFO, ACE_TEXT("%IGoodnight\n")));
	return EXIT_SUCCESS;
}
