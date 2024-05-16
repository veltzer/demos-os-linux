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
#include <ace/OS_NS_sys_time.h>
#include <ace/os_include/os_netdb.h>
#include <ace/Log_Msg.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Reactor.h>
#include <ace/Acceptor.h>
#include <stdlib.h>	// for EXIT_SUCCESS
#include "ClientService.hh"

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 * EXTRA_LINK_FLAGS_AFTER=-lACE -ldl -lrt -lpthread
 */

typedef ACE_Acceptor<ClientService, ACE_SOCK_ACCEPTOR> ClientAcceptor;

int ClientService::open(void* p) {
	if(super::open(p)==-1) {
		return -1;
	}
	ACE_TCHAR peer_name[MAXHOSTNAMELEN];
	ACE_INET_Addr peer_addr;
	if((this->peer().get_remote_addr(peer_addr)==0) && (peer_addr.addr_to_string(peer_name, MAXHOSTNAMELEN)==0)) {
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Connection from %s\n"), peer_name));
	}
	return 0;
}

int ClientService::handle_input(ACE_HANDLE) {
	const size_t INPUT_SIZE=4096;
	char buffer[INPUT_SIZE];
	ssize_t recv_cnt, send_cnt;

	recv_cnt=this->peer().recv(buffer, sizeof(buffer));
	if(recv_cnt<=0) {
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P|%t) Connection closed\n")));
		return -1;
	}
	send_cnt=this->peer().send(buffer, static_cast<size_t>(recv_cnt));
	if(send_cnt==recv_cnt) {
		return 0;
	}
	if((send_cnt==-1) && (ACE_OS::last_error()!=EWOULDBLOCK)) {
		ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("(%P|%t) %p\n"), ACE_TEXT("send")), 0);
	}
	if(send_cnt==-1) {
		send_cnt=0;
	}
	ACE_Message_Block *mb=0;
	size_t remaining=static_cast<size_t>((recv_cnt - send_cnt));
	ACE_NEW_RETURN(mb, ACE_Message_Block(remaining), -1);
	mb->copy(&buffer[send_cnt], remaining);
	int output_off=this->msg_queue()->is_empty();
	ACE_Time_Value nowait(ACE_OS::gettimeofday());
	if(this->putq(mb, &nowait)==-1) {
		ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) %p; discarding data\n"), ACE_TEXT("enqueue failed")));
		mb->release();
		return 0;
	}
	if(output_off) {
		return this->reactor()->register_handler(this, ACE_Event_Handler::WRITE_MASK);
	}
	return 0;
}

int ClientService::handle_output(ACE_HANDLE) {
	ACE_Message_Block *mb=0;

	ACE_Time_Value nowait(ACE_OS::gettimeofday());
	while(-1!=this->getq(mb, &nowait)) {
		ssize_t send_cnt=this->peer().send(mb->rd_ptr(), mb->length());
		if(send_cnt==-1) {
			ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) %p\n"), ACE_TEXT("send")));
		} else {
			mb->rd_ptr(static_cast<size_t>(send_cnt));
		}
		if(mb->length() > 0) {
			this->ungetq(mb);
			break;
		}
		mb->release();
	}
	return (this->msg_queue()->is_empty()) ? -1 : 0;
}

int ClientService::handle_close(ACE_HANDLE h, ACE_Reactor_Mask mask) {
	if(mask==ACE_Event_Handler::WRITE_MASK) {
		return 0;
	}
	return super::handle_close(h, mask);
}

int main() {
	ACE_INET_Addr port_to_listen("HAStatus");
	ClientAcceptor acceptor;
	if(acceptor.open(port_to_listen, ACE_Reactor::instance(), ACE_NONBLOCK)==-1) {
		return 1;
	}
	ACE_Reactor::instance()->run_reactor_event_loop();
	return EXIT_SUCCESS;
}
