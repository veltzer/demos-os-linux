/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <ace/Task.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>
#include <ace/Log_Msg.h>
#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_stdio.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 */

class SocketHandler : public ACE_Task_Base {
public:
	void set_value(int ivalue) {
		value=ivalue;
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Setting value %d\n"), value));
	}
	virtual int svc(void) {
		ACE_INET_Addr addr;
		ACE_SOCK_Connector connector;
		ACE_SOCK_Stream peer;
		int bc;
		static char buf[64];
		ACE_Time_Value timeout(10, 0);
		int i=0;
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) SocketHandler Thread %d running\n"), value));
		addr.set(50000 + value, ACE_LOCALHOST);
		if(-1==connector.connect(peer, addr, &timeout)) {
			ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("Client connect")), 1);
		}
		while(true) {
			bc=peer.recv(buf, sizeof(buf));
			peer.send_n("OK", 3, 0);
			buf[bc]=0;
			if (!ACE_OS::strcmp(buf, "End")) {
				printf("Client: <%d> End\n", value);
				peer.close();
				return(0);
			}
			printf("Client: <%d> %2d. %s", value, ++i, buf);
			ACE_OS::sleep(1);
		}
		return(0);
	}

private:
	int value;
};

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	SocketHandler handler[3];
	for(int i=0; i<3; i++) {
		handler[i].set_value(i);
		handler[i].activate();
	}
	for(int i=0; i<3; i++) {
		handler[i].wait();
	}
	return EXIT_SUCCESS;
}
