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
#include <ace/OS_NS_string.h>
#include <ace/Get_Opt.h>
#include <ace/Log_Msg.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

int main(int argc, char** argv) {
	static const char* options=":f:";

	ACE_Get_Opt cmd_opts(argc, argv, options, 1, 0, ACE_Get_Opt::PERMUTE_ARGS, 1);
	if (cmd_opts.long_option("config", 'f', ACE_Get_Opt::ARG_REQUIRED)==-1) {
		return -1;
	}
	int option;
	char config_file[MAXPATHLEN];
	ACE_OS_String::strcpy(config_file, "HAStatus.conf");
	while((option=cmd_opts())!=EOF) {
		switch (option) {
		case 'f':
			ACE_OS_String::strncpy(config_file, cmd_opts.opt_arg(), MAXPATHLEN);
			break;

		case ':':
			ACE_ERROR_RETURN ((LM_ERROR, "-%c requires an argument\n", cmd_opts.opt_opt()), -1);
			break;

		default:
			ACE_ERROR_RETURN ((LM_ERROR, "Parse error.\n"), -1);
			break;
		}
	}
	ACE_DEBUG((LM_DEBUG, "Config file is %s\n", config_file));
	return EXIT_SUCCESS;
}
