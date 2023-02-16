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
// the next define makes the trace work...
#define ACE_NTRACE 1
#include <ace/Log_Msg.h>

/*
 * This demo shows the ACE logging facilities
 *
 * ACE_NTRACE: supports whether ACE_TRACE actually does anything. If disabled then ACE_TRACE
 * is nothing. If enabled then:
 * - traces will be printed where the are in the code (ACE_TRACE).
 * - traces will be printed on exit from functions (destructor pattern).
 * - %I can be used in ACE_DEBUG in order to indent messages properly.
 * - functions that do not use ACE_TRACE in them will not show up or contribute
 *	to the trace nesting at all.
 *
 * note that ACE_NTRACE can be enabled/disabled on a per file basis and so can
 * only be enabled at compile time in modules that you want to debug...
 *
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

void bar(void) {
	ACE_TRACE(ACE_TEXT("bar"));
	ACE_DEBUG((LM_INFO, ACE_TEXT("%IMessage from bar\n")));
}

void foo(void) {
	ACE_DEBUG((LM_INFO, ACE_TEXT("%IMessage from foo\n")));
	bar();
}

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	ACE_TRACE(ACE_TEXT("main"));
	ACE_DEBUG((LM_INFO, ACE_TEXT("%IBefore\n")));
	foo();
	ACE_DEBUG((LM_INFO, ACE_TEXT("%IAfter\n")));
	return EXIT_SUCCESS;
}
