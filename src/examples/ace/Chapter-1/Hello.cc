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
#include <ace/Log_Msg.h>// for ACE_DEBUG and ACE_ERROR
#include <ace/OS_NS_stdio.h>	// for the printf function
#include <ace/streams.h>// for C++ streams interface
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * Short program demonstrates printing Hello World using the ACE environment
 * notice that both ACE_DEBUG and ACE_ERROR are going to stderr.
 *
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

// Notice the use of the ACE version of main
int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	printf("It seems that printf is working\n");
	cout << "It seems like C++ streams are working without std:: prefix" << endl;
	// Notice that ACE_DEBUG takes one argument
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("ACE_DEBUG(pid=%P Time:%T)\n")));
	ACE_ERROR((LM_ERROR, ACE_TEXT("ACE_ERROR(pid=%P File:%N Line:%l Date+Time:%D)\n")));
	return EXIT_SUCCESS;
}
