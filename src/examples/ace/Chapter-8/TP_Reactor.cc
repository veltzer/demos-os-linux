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

#include <firstinclude.h>
#include <ace/config-lite.h>
#include <ace/OS_NS_string.h>
#include <ace/OS_NS_unistd.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Acceptor.h>
#include <ace/Thread_Manager.h>
#include <ace/TP_Reactor.h>
#include <stdlib.h>	// for EXIT_SUCCESS
#include "Request_Handler.hh"

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 * EXTRA_LINK_FLAGS_AFTER=-lpthread
 */

// Accepting end point. This is actually "localhost:10010", but some
// platform couldn't resolve the name so we use the IP address
// directly here.
static const char *rendezvous="127.0.0.1:10010";

// Total number of server threads.
static size_t svr_thrno=5;

// Total number of client threads.
static size_t cli_runs=2;

// Total connection attemps of a client thread.
static size_t cli_conn_no=2;

// Total requests a client thread sends.
static size_t cli_req_no=5;

// Delay before a thread sending the next request (in msec.)
static int req_delay=50;

typedef ACE_Strategy_Acceptor<Request_Handler, ACE_SOCK_ACCEPTOR> ACCEPTOR;

Request_Handler::Request_Handler(ACE_Thread_Manager *thr_mgr) : ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_MT_SYNCH> (thr_mgr), nr_msgs_rcvd_(0) {
	this->reactor(ACE_Reactor::instance());
}

int Request_Handler::handle_input(ACE_HANDLE fd) {
	char buffer[BUFSIZ];
	char len=0;
	ssize_t result=this->peer().recv(&len, sizeof(char));
	if ((result > 0) && (this->peer().recv_n(buffer, len * sizeof(char))==static_cast<ssize_t>(len * sizeof(char)))) {
		++this->nr_msgs_rcvd_;
		ACE_DEBUG((LM_DEBUG, "(%t) svr input; fd: 0x%x; input: %s\n", fd, buffer));
		if (ACE_OS::strcmp(buffer, "shutdown")==0) {
			ACE_Reactor::instance()->end_reactor_event_loop();
		}
		return 0;
	} else {
		ACE_DEBUG((LM_DEBUG, "(%t) Request_Handler: 0x%x peer closed (0x%x)\n", this, fd));
	}
	return -1;
}

int Request_Handler::handle_close(ACE_HANDLE fd, ACE_Reactor_Mask) {
	ACE_DEBUG((LM_DEBUG, "(%t) svr close; fd: 0x%x, rcvd %d msgs\n", fd, this->nr_msgs_rcvd_));
	if (this->nr_msgs_rcvd_!=cli_req_no) {
		ACE_ERROR((LM_ERROR, "(%t) Handler 0x%x: Expected %d messages; got %d\n", this, cli_req_no, this->nr_msgs_rcvd_));
	}
	this->destroy();
	return 0;
}

// Listing 2 code/ch16
static int reactor_event_hook(ACE_Reactor *) {
	ACE_DEBUG((LM_DEBUG, "(%t) handling events ....\n"));
	return 0;
}

class ServerTP: public ACE_Task_Base {
public:
	virtual int svc(void) {
		ACE_DEBUG((LM_DEBUG, "(%t) Running the event loop\n"));
		int result=ACE_Reactor::instance()->run_reactor_event_loop(&reactor_event_hook);
		if (result==-1) {
			ACE_ERROR_RETURN((LM_ERROR, "(%t) %p\n", "Error handling events"), 0);
		}
		ACE_DEBUG((LM_DEBUG, "(%t) Done handling events.\n"));
		return 0;
	}
};
// Listing 2

class Client: public ACE_Task_Base {
public:
	Client() : addr_(rendezvous) {
	}
	virtual int svc() {
		ACE_OS::sleep(3);
		const char* msg="Message from Connection worker";
		char buf[BUFSIZ];
		buf[0]=ACE_OS::strlen(msg) + 1;
		ACE_OS::strcpy(&buf[1], msg);
		for(size_t i=0; i<cli_runs; i++) {
			send_work_to_server(buf);
		}
		shut_down();
		return 0;
	}

private:
	void send_work_to_server(char* arg) {
		ACE_SOCK_Stream stream;
		ACE_SOCK_Connector connect;
		ACE_Time_Value delay(0, req_delay);
		size_t len=*reinterpret_cast<char*>(arg);
		for(size_t i=0; i<cli_conn_no; i++) {
			if(connect.connect(stream, addr_) < 0) {
				ACE_ERROR((LM_ERROR, "(%t) %p\n", "connect"));
				continue;
			}
			for(size_t j=0; j<cli_req_no; j++) {
				ACE_DEBUG((LM_DEBUG, "Sending work to server on handle 0x%x, req %d\n", stream.get_handle(), j + 1));
				if (stream.send_n(arg, (len + 1) * sizeof(char))==-1) {
					ACE_ERROR((LM_ERROR, "(%t) %p\n", "send_n"));
					continue;
				}
				ACE_OS::sleep(delay);
			}
			stream.close();
		}
	}
	void shut_down() {
		ACE_SOCK_Stream stream;
		ACE_SOCK_Connector connect;
		if (connect.connect(stream, addr_)==-1) {
			ACE_ERROR((LM_ERROR, "(%t) %p Error while connecting\n", "connect"));
		}
		const char* sbuf="\011shutdown";
		ACE_DEBUG((LM_DEBUG, "shutdown stream handle=%x\n", stream.get_handle()));
		if (stream.send_n(sbuf, (ACE_OS::strlen(sbuf) + 1) * sizeof(char))==-1) {
			ACE_ERROR((LM_ERROR, "(%t) %p\n", "send_n"));
		}
		stream.close();
	}

private:
	ACE_INET_Addr addr_;
};

int main() {
	ACE_TP_Reactor sr;
	ACE_Reactor new_reactor(&sr);
	ACE_Reactor::instance(&new_reactor);
	ACCEPTOR acceptor;
	ACE_INET_Addr accept_addr(rendezvous);
	if(acceptor.open(accept_addr)==-1) {
		ACE_ERROR_RETURN((LM_ERROR, "%p\n", "open"), 1);
	}
	ACE_DEBUG((LM_DEBUG, "(%t) Spawning %d server threads...\n", svr_thrno));
	ServerTP serverTP;
	serverTP.activate(THR_NEW_LWP | THR_JOINABLE, svr_thrno);
	Client client;
	client.activate();
	ACE_Thread_Manager::instance()->wait();
	return EXIT_SUCCESS;
}
