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
#include <ace/os_include/os_netdb.h>
#include <ace/OS_NS_string.h>
#include <ace/Log_Msg.h>
#include <ace/ARGV.h>
#include <ace/Get_Opt.h>

/*
 * ACE_ARGV examples not in a larger program. Sample code from The ACE
 * Programmer's Guide, Copyright 2003 Addison-Wesley. All Rights Reserved.
 *
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

int main() {
	static const char* options=":f:h:";
	static const char* cmdline="-f /home/managed.cfg -h $HOSTNAME";

	ACE_ARGV cmdline_args(cmdline);
	ACE_Get_Opt cmd_opts(cmdline_args.argc(), cmdline_args.argv(), options, 0);
	// Don't skip any args

	int option;
	char config_file[MAXPATHLEN];
	char hostname[MAXHOSTNAMELEN];

	ACE_OS_String::strcpy(config_file, "HAStatus.conf");
	ACE_OS_String::strcpy(hostname, "not set");
	while((option=cmd_opts())!=EOF) {
		switch (option) {
		case 'f':
			ACE_OS_String::strncpy(config_file, cmd_opts.opt_arg(), MAXPATHLEN);
			break;

		case 'h':
			ACE_OS_String::strncpy(hostname, cmd_opts.opt_arg(), MAXHOSTNAMELEN);
			break;

		case ':':
			ACE_ERROR_RETURN ((LM_ERROR, "-%c requires an argument\n", cmd_opts.opt_opt()), -1);
			break;

		default:
			ACE_ERROR_RETURN ((LM_ERROR, "Parse error.\n"), -1);
			break;
		}
	}
	ACE_DEBUG((LM_DEBUG, "Config file: %s\n", config_file));
	ACE_DEBUG((LM_DEBUG, "Hostname: %s\n", hostname));
	return EXIT_SUCCESS;
}
