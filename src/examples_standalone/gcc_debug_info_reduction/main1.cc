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

#define ACE_NTRACE 0
#include <ace/Log_Msg.h>// for ACE_TRACE(), ACE_DEBUG()
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This is a basic example of using ACE_DEBUG and ACE_TRACE.
 *
 * Notes:
 * - the ACE_NTRACE macro controls whether trace does anything special or
 * not.
 * - the "%I" ACE_DEBUG modifier adds indentation level which matches the trace
 * level.
 * - you have to put your own newline at the end of ACE_DEBUG messages.
 *
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */
#ifdef DO_PRAGMA
#pragma
#endif	/* DO_PRAGMA */
void foo(void) {
	ACE_TRACE("foo");
	ACE_DEBUG((LM_INFO, "%IHowdy Pardner\n"));
}

int main() {
	ACE_TRACE("main");
	ACE_DEBUG((LM_INFO, "%IHi Mom\n"));
	foo();
	ACE_DEBUG((LM_INFO, "%IGoodnight\n"));
	foo();
	return EXIT_SUCCESS;
}
