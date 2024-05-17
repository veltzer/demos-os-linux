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
#define ACE_NTRACE 0
#include <ace/Log_Msg.h>
#include <stdlib.h>	// for EXIT_SUCCESS
#include "Callback.hh"

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

int main() {
	Callback* callback=new Callback;
	ACE_LOG_MSG->set_flags(ACE_Log_Msg::MSG_CALLBACK);
	ACE_LOG_MSG->clr_flags(ACE_Log_Msg::STDERR);
	ACE_LOG_MSG->msg_callback(callback);
	ACE_TRACE("main");
	ACE_DEBUG((LM_DEBUG, "%IHi Mom\n"));
	ACE_DEBUG((LM_INFO, "%IGoodnight\n"));
	return EXIT_SUCCESS;
}
