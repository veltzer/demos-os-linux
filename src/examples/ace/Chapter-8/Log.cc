/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <ace/Log_Msg.h>// for ACE_DEBUG(), ACE_LOG_MSG, ACE_MAIN, ACE_TCHAR
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This is an example of how to use the syslog facilities of Linux via the
 * ACE API.
 *
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 */

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	// log to stderr
	ACE_DEBUG((LM_INFO, ACE_TEXT("use [tail /var/log/syslog] to see next entries\n")));
	// starting logging to syslog
	ACE_LOG_MSG->open(argv[0], ACE_Log_Msg::SYSLOG, ACE_TEXT("syslogTest"));
	ACE_DEBUG((LM_INFO, ACE_TEXT("%IThis is a message to syslog\n")));
	// now back to the command line
	ACE_LOG_MSG->open(argv[0]);
	ACE_DEBUG((LM_INFO, ACE_TEXT("argv[0]=%s\n"), argv[0]));
	return EXIT_SUCCESS;
}
