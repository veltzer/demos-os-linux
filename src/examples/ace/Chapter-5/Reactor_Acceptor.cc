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
#include <ace/INET_Addr.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Reactor.h>
#include <ace/Log_Msg.h>
#include <ace/SOCK_Stream.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example shows how to use a socket acceptor through a reactor
 *
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

class ClientAcceptor : public ACE_Event_Handler {
public:
	int open(const ACE_INET_Addr& listen_addr);
	virtual ~ClientAcceptor();

	virtual ACE_HANDLE get_handle(void) const;

	virtual int handle_input(ACE_HANDLE fd=ACE_INVALID_HANDLE);

protected:
	ACE_SOCK_Acceptor m_acceptor;
};

int ClientAcceptor::open(const ACE_INET_Addr& listen_addr) {
	if(this->m_acceptor.open(listen_addr, 1)==-1) {
		return -1;
	}
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) connected\n")));
	return(this->reactor()->register_handler(this, ACE_Event_Handler::ACCEPT_MASK));
}

ClientAcceptor::~ClientAcceptor() {
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) destructor\n")));
	this->handle_close(ACE_INVALID_HANDLE, 0);
}

// this is the 'magic' method, it is the way by which the reactor
// is "connected" to the socket that the acceptor gets from the operating
// system when calling 'socket(2)'...
ACE_HANDLE ClientAcceptor::get_handle(void) const {
	return(this->m_acceptor.get_handle());
}
int ClientAcceptor::handle_input(ACE_HANDLE) {
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) handle_input\n")));
	ACE_SOCK_Stream peer;
	if(this->m_acceptor.accept(peer)==-1) {
		ACE_ERROR_RETURN((LM_ERROR, "%p\n","Failed to accept client connection"), -1);
	}
	peer.close();
	return(0);
}

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	// create the acceptor
	ClientAcceptor acceptor;
	// register the acceptor with the singleton reactor
	acceptor.reactor(ACE_Reactor::instance());
	// open the socket on a specific port
	const unsigned int port=50000;
	ACE_INET_Addr port_to_listen(port);
	if(acceptor.open(port_to_listen)==-1) {
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("acceptor.open")), -1);
	}
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) connect to me using 'telnet localhost %d'\n"), port));
	// main loop on the singleton reactor
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) before event loop\n")));
	ACE_Reactor::instance()->run_reactor_event_loop();
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) after event loop\n")));
	return EXIT_SUCCESS;
}
