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
#include <ace/SOCK_Acceptor.h>
#include <ace/Log_Msg.h>
#include <ace/Read_Buffer.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 */

// a Read_Buffer which is always connected to STDIN
static ACE_Read_Buffer rb(ACE_STDIN);

int GetMessageType(char* data) {
	// read a single line from stdin
	// Allocate a new buffer.
	char* buffer=rb.read('\n');
	if(buffer==0) {
		// return message type zero when EOF is reached
		return(0);
	} else {
		int type;
		sscanf(buffer, "%d", &type);
		// Remove the type from the buffer
		ACE_OS::sprintf(data, "%s", buffer+2);
		return(type);
	}
}

int MakeConnection(ACE_SOCK_Acceptor *acceptor, ACE_INET_Addr *port_to_listen, ACE_SOCK_Stream *peer, ACE_INET_Addr *peer_addr) {
	/*
	 * Basic acceptor usage - No timeout
	 */
#define NO_TIMEOUT
#ifdef NO_TIMEOUT
	if (acceptor->accept(*peer)==-1) {
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("(%P|%t) Failed to accept ") ACE_TEXT("client connection\n")), 100);
	}
#else
	ACE_Time_Value timeout(10, 0);
	if (acceptor->accept(*peer, peer_addr, &timeout, 0)==-1) {
		if (ACE_OS::last_error()==EINTR) {
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Interrupted while ") ACE_TEXT("waiting for connection\n")));
		} else if (ACE_OS::last_error()==ETIMEDOUT) {
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Timeout while ") ACE_TEXT("waiting for connection\n")));
		}
	} else {
		ACE_TCHAR peer_name[MAXHOSTNAMELEN];
		peer_addr->addr_to_string(peer_name, MAXHOSTNAMELEN);
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Connection from %s\n"), peer_name));
	}
#endif	/* NO_TIMEOUT */
	return(0);
}

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	ACE_INET_Addr port_to_listen[3];
	ACE_SOCK_Acceptor acceptor[3];
	ACE_INET_Addr peer_addr[3];
	ACE_SOCK_Stream peer[3];
	int type=1;
	char buffer[4096];
	for(int i=0; i<3; i++) {
		port_to_listen[i]=ACE_INET_Addr(50000 + i, ACE_LOCALHOST);
		if (acceptor[i].open(port_to_listen[i], 1)==-1) {
			ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("acceptor.open")), 100);
		}
	}
	for(int i=0; i<3; i++) {
		MakeConnection(&acceptor[i], &port_to_listen[i], &peer[i], &peer_addr[i]);
	}
	int i;
	while(type!=0) {
		type=GetMessageType(buffer);
		if (type) {
			size_t size=ACE_OS::strlen(buffer);
			buffer[size++]='\n';
			if((type==1) || (type==2)) {
				i=0;
			} else {
				if ((type==3) || (type==4)) {
					i=1;
				} else {
					i=2;
				}
			}
			peer[i].send_n(buffer, size, 0);
			peer[i].recv(buffer, sizeof(buffer));
		}
	}
	// send "End" and close the connections
	for(int i=0; i<3; i++) {
		peer[i].send_n("End", 4, 0);
		peer[i].recv(buffer, sizeof(buffer));
		peer[i].close();
	}
	return EXIT_SUCCESS;
}
