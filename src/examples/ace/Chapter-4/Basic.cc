/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>
#include <ace/Log_Msg.h>
#include <ace/OS_NS_unistd.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 */

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	ACE_INET_Addr srvr(50000, ACE_LOCALHOST);
	ACE_SOCK_Connector connector;
	ACE_SOCK_Stream peer;
	if(-1==connector.connect(peer, srvr)) {
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("connect")), 1);
	}
	ssize_t bc;
	char buf[64];
	peer.send_n("uptime\n", 7);
	bc=peer.recv(buf, sizeof(buf));
	ACE_OS::write(ACE_STDOUT, buf, bc);
	peer.close();
	return EXIT_SUCCESS;
}
