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
#include <ace/OS_NS_errno.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>
#include <ace/Log_Msg.h>
#include <ace/Time_Value.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	/*
	 * Here we will use the default ctor and the set()
	 * method to configure it. After each set() we will
	 * display the address as a string and then connect
	 * to each respective server. We can reuse the addr
	 * instance once connection has been established.
	 *
	 * // Listing 1 code/ch06
	 * ACE_INET_Addr addr;
	 * ...
	 * addr.set ("HAStatus", ACE_LOCALHOST);
	 * ...
	 * addr.set ("HALog", ACE_LOCALHOST);
	 * // Listing 1
	 *
	 */

	ACE_INET_Addr addr;
	ACE_TCHAR peerAddress[64];

	// Listing 2 code/ch06
	addr.set(ACE_TEXT("HAStatus"), ACE_LOCALHOST);
	if (addr.addr_to_string(peerAddress, sizeof(peerAddress), 0)==0) {
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Connecting to %s\n"), peerAddress));
	}
	// Listing 2

	// Listing 3 code/ch06
	ACE_SOCK_Stream status;
	ACE_OS::last_error(0);
	ACE_SOCK_Connector statusConnector(status, addr);
	if (ACE_OS::last_error()) {
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("status")), 100);
	}
	// Listing 3

	addr.set(ACE_TEXT("HALog"), ACE_LOCALHOST);
	if (addr.addr_to_string(peerAddress, sizeof(peerAddress), 0)==0) {
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Connecting to %s\n"), peerAddress));
	}
	// Listing 4 code/ch06
	ACE_SOCK_Connector logConnector;
	ACE_Time_Value timeout(10);

	ACE_SOCK_Stream log;
	if (logConnector.connect(log, addr, &timeout)==-1) {
		if (ACE_OS::last_error()==ETIME) {
			ACE_DEBUG((LM_DEBUG, "Timeout while connecting to log server\n"));
		} else {
			ACE_ERROR((LM_ERROR, "%p\nlog"));
		}
		return(101);
	}
	// Listing 4

	/*
	 * We generally let the OS pick our local port number but
	 * if you want, you can choose that also:
	 * // Listing 5 code/ch06
	 * ACE_SOCK_Connector logConnector;
	 * ACE_INET_Addr local (4200, ACE_LOCALHOST);
	 * if (logConnector.connect (log, addr, 0, local)==-1)
	 * {
	 * ...
	 * // Listing 5
	 * }
	 */

	char buf[64];

	// Listing 6 code/ch06
	ACE_Time_Value sendTimeout(0, 5);
	if (status.send_n("uptime\n", 7, &sendTimeout)==-1) {
		if (ACE_OS::last_error()==ETIME) {
			ACE_DEBUG((LM_DEBUG, "Timeout while sending"));
		}
		// Listing 6
		else {
			ACE_ERROR((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("send_n")));
		}
		return(102);
	}
	ssize_t bc;
	ACE_Time_Value recvTimeout(0, 1);
	if((bc=status.recv(buf, sizeof(buf), &recvTimeout))==-1) {
		ACE_ERROR((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("recv")));
		return(103);
	}
	log.send_n(buf, bc);
	status.close();
	log.close();
	return EXIT_SUCCESS;
}
