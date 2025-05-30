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
#include <ace/Log_Msg.h>// for ACE_DEBUG(), ACE_LOG_MSG
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This is an example of how to use the syslog facilities of Linux via the
 * ACE API.
 *
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

int main(int, char** argv) {
	// log to stderr
	ACE_DEBUG((LM_INFO, "use [tail /var/log/syslog] to see next entries\n"));
	// starting logging to syslog
	ACE_LOG_MSG->open(argv[0], ACE_Log_Msg::SYSLOG, "syslogTest");
	ACE_DEBUG((LM_INFO, "%IThis is a message to syslog\n"));
	// now back to the command line
	ACE_LOG_MSG->open(argv[0]);
	ACE_DEBUG((LM_INFO, "argv[0]=%s\n", argv[0]));
	return EXIT_SUCCESS;
}
