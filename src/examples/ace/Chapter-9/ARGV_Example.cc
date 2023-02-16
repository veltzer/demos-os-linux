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

int ACE_TMAIN(int argc, ACE_TCHAR** argv) {
	static const ACE_TCHAR options[]=ACE_TEXT(":f:h:");
	static const ACE_TCHAR cmdline[]=ACE_TEXT("-f /home/managed.cfg -h $HOSTNAME");

	ACE_ARGV cmdline_args(cmdline);
	ACE_Get_Opt cmd_opts(cmdline_args.argc(), cmdline_args.argv(), options, 0);
	// Don't skip any args

	int option;
	ACE_TCHAR config_file[MAXPATHLEN];
	ACE_TCHAR hostname[MAXHOSTNAMELEN];

	ACE_OS_String::strcpy(config_file, ACE_TEXT("HAStatus.conf"));
	ACE_OS_String::strcpy(hostname, ACE_TEXT("not set"));
	while((option=cmd_opts())!=EOF) {
		switch (option) {
		case 'f':
			ACE_OS_String::strncpy(config_file, cmd_opts.opt_arg(), MAXPATHLEN);
			break;

		case 'h':
			ACE_OS_String::strncpy(hostname, cmd_opts.opt_arg(), MAXHOSTNAMELEN);
			break;

		case ':':
			ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT("-%c requires an argument\n"), cmd_opts.opt_opt()), -1);
			break;

		default:
			ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT("Parse error.\n")), -1);
			break;
		}
	}
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Config file: %s\n"), config_file));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Hostname: %s\n"), hostname));
	return(0);
}
