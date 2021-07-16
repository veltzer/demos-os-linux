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
#define ACE_NTRACE 0
#include <ace/Log_Msg.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * Exercise 1
 * 1. Write a program that will have two functions FunA and FunB.
 *	use a loop counter to call twice FunA and this function will call FunB.
 * 2. Each function will print a message when entered, using the ACE utility.
 * 3. Use the ACE to set and unset this capability.
 * 4. In each function print debugging information using 3 levels:
 *	1. LM_TRACE
 *	2. LM_DEBUG
 *	3. LM_INFO
 *	Use the following options in each output line:
 *	1. Print program pid (%P).
 *	2. Print line number (%l).
 *	3. Print File Name (%N).
 *
 * Use %I to cause indentation. without trace this does nothing.
 * With trace enabled it will indent according to functions entered.
 * Multiple "%I" usage is possible.
 *
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 */

void FunB() {
	// ACE_TRACE(ACE_TEXT("FunB"));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("%I DEBUG: pid=%P Line:%l File=%N\n")));
	// ACE_DEBUG((LM_INFO,ACE_TEXT("%I INFO: pid=%P Line:%l File=%N\n")));
	// ACE_DEBUG((LM_NOTICE,ACE_TEXT("%I NOTICE: pid=%P Line:%l File=%N\n")));
}

void FunA() {
	// ACE_TRACE(ACE_TEXT("FunA"));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("%I DEBUG: pid=%P Line:%l File=%N\n")));
	// ACE_DEBUG((LM_INFO,ACE_TEXT("%I INFO: pid=%P Line:%l File=%N\n")));
	// ACE_DEBUG((LM_NOTICE,ACE_TEXT("%I NOTICE: pid=%P Line:%l File=%N\n")));
	FunB();
}

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	ACE_LOG_MSG->priority_mask(LM_DEBUG | LM_INFO | LM_NOTICE | LM_TRACE, ACE_Log_Msg::PROCESS);
	ACE_TRACE(ACE_TEXT("main"));
	for(int i=0; i<2; i++) {
		FunA();
	}
	return EXIT_SUCCESS;
}
