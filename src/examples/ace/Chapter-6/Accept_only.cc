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
#include <ace/OS_NS_sys_time.h>
#include <ace/os_include/os_netdb.h>
#include <ace/Auto_Ptr.h>
#include <ace/Log_Msg.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Reactor.h>
#include <ace/Message_Block.h>
#include <ace/Message_Queue.h>
#include <ace/SOCK_Stream.h>
#include <ace/Synch.h>
#include <ace/Signal.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example only accepts connections.
 *
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

class ClientAcceptor: public ACE_Event_Handler {
public:
	virtual~ClientAcceptor() {
		// this->handle_close(ACE_INVALID_HANDLE, 0);
	}

	int open(const ACE_INET_Addr& listen_addr) {
		if(this->acceptor_.open(listen_addr, 1)==-1) {
			ACE_ERROR_RETURN((LM_ERROR, "%p\n", "acceptor.open"), -1);
		}
		return this->reactor()->register_handler(this, ACE_Event_Handler::ACCEPT_MASK);
	}

	// Get this handler's I/O handle.
	virtual ACE_HANDLE get_handle(void) const{
		return this->acceptor_.get_handle();
	}

	// Called when a connection is ready to accept.
	virtual int handle_input(ACE_HANDLE fd __attribute__((unused))=ACE_INVALID_HANDLE) {
		ACE_DEBUG((LM_DEBUG, "(%P|%t) got handle_input\n"));
		ACE_OS::sleep(1);
		// this means 'deregister this handling method'
		return -1;
	}

	// Called when this handler is removed from the ACE_Reactor.
	virtual int handle_close(ACE_HANDLE, ACE_Reactor_Mask) {
		return 0;
	}

protected:
	ACE_SOCK_Acceptor acceptor_;
};

class LoopStopper: public ACE_Event_Handler {
public:
	LoopStopper(int signum) {
		ACE_Reactor::instance()->register_handler(signum, this);
	}

	// Called when object is signaled by OS.
	virtual int handle_signal(int signum __attribute__((unused)), siginfo_t* =0, ucontext_t* =0) {
		ACE_Reactor::instance()->end_reactor_event_loop();
		return 0;
	}
};

int main() {
	const unsigned int port=8081;
	ACE_INET_Addr port_to_listen(port);
	ACE_DEBUG((LM_DEBUG, "(%P|%t) Connect to me using [telnet localhost:%d]\n", port));
	ACE_DEBUG((LM_DEBUG, "(%P|%t) Kill me using [kill -s SIGINT %P]\n", port));

	ClientAcceptor acceptor;
	acceptor.reactor(ACE_Reactor::instance());
	if(acceptor.open(port_to_listen)==-1) {
		return 1;
	}
	LoopStopper loopstopper(SIGINT);

	ACE_Reactor::instance()->run_reactor_event_loop();
	return EXIT_SUCCESS;
}
