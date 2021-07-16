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
#include <ace/OS_NS_string.h>
#include <ace/Get_Opt.h>
#include <ace/Log_Msg.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 */

//
// options are: -f file FileName
// -g
// -h
// -l parameter
// --cool_option - we get 0 into the switch
// --the_answer - we get 42 into the switch
//
//
int ACE_TMAIN(int argc, ACE_TCHAR** argv) {
	// Example for a long option without a corresponding short option.
	// Just put some context here so the following compiles and runs.
	static const ACE_TCHAR options[]=ACE_TEXT(":f:l:gh");
	ACE_Get_Opt cmd_opts(argc, argv, options);
	cmd_opts.long_option(ACE_TEXT("cool_option"));
	cmd_opts.long_option(ACE_TEXT("the_answer"), 42);
	int option;
	ACE_TCHAR config_file[MAXPATHLEN];
	ACE_OS_String::strcpy(config_file, ACE_TEXT("HAStatus.conf"));
	while((option=cmd_opts())!=EOF) {
		switch (option) {
		case 'f':
			ACE_OS_String::strncpy(config_file, cmd_opts.opt_arg(), MAXPATHLEN);
			ACE_DEBUG((LM_DEBUG, "-f: The new config_file: %s\n", config_file));
			break;
		case 'g':
			ACE_DEBUG((LM_DEBUG, "-g: is selected\n"));
			break;
		case 'h':
			ACE_DEBUG((LM_DEBUG, "-h: is selected\n"));
			break;
		case 'l':
			ACE_DEBUG((LM_DEBUG, "-l: is selected we ignore the optional parameter.\n"));
			break;
		case 0:
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("Yes, very cool.\n")));
			break;
		case 42:
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("the_answer is 42\n")));
			break;
		case ':':
			ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("-%c requires an argument\n"), cmd_opts.opt_opt()), -1);
			break;
		default:
			ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("Parse error.\n")), -1);
			break;
		}
	}
	return EXIT_SUCCESS;
}
