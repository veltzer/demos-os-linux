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
#include <ace/OS_NS_sys_time.h>
#include <ace/os_include/os_netdb.h>
#include <ace/Log_Msg.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Reactor.h>
#include <ace/Message_Queue.h>
#include <ace/SOCK_Stream.h>
#include <ace/Synch.h>
#include <ace/Signal.h>
#include <cstdlib>	// for EXIT_SUCCESS
#include <memory>

using namespace std;

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 * EXTRA_LINK_FLAGS_AFTER=-lACE -ldl -lrt -lpthread
 */

class ClientAcceptor: public ACE_Event_Handler {
public:
	virtual~ClientAcceptor();
	int open(const ACE_INET_Addr& listen_addr);
	virtual ACE_HANDLE get_handle(void) const{
		return this->acceptor_.get_handle();
	}
	// Called when a connection is ready to accept.
	virtual int handle_input(ACE_HANDLE fd=ACE_INVALID_HANDLE);
	// Called when this handler is removed from the ACE_Reactor.
	virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

protected:
	ACE_SOCK_Acceptor acceptor_;
};

class ClientService: public ACE_Event_Handler {
public:
	ACE_SOCK_Stream& peer(void) {
		return this->sock_;
	}
	int open(void);
	// Get this handler's I/O handle.
	virtual ACE_HANDLE get_handle(void) const{
		return this->sock_.get_handle();
	}
	// Called when input is available from the client.
	virtual int handle_input(ACE_HANDLE fd=ACE_INVALID_HANDLE);
	// Called when output is possible.
	virtual int handle_output(ACE_HANDLE fd=ACE_INVALID_HANDLE);
	// Called when this handler is removed from the ACE_Reactor.
	virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

protected:
	ACE_SOCK_Stream sock_;
	ACE_Message_Queue<ACE_NULL_SYNCH> output_queue_;
};

ClientAcceptor::~ClientAcceptor() {
	this->handle_close(ACE_INVALID_HANDLE, 0);
}

int ClientAcceptor::open(const ACE_INET_Addr& listen_addr) {
	if(this->acceptor_.open(listen_addr, 1)==-1) {
		ACE_ERROR_RETURN((LM_ERROR, "%p\n", "acceptor.open"), -1);
	}
	return this->reactor()->register_handler (this, ACE_Event_Handler::ACCEPT_MASK);
}

int ClientAcceptor::handle_input(ACE_HANDLE) {
	ClientService* client;
	ACE_NEW_RETURN(client, ClientService, -1);
	unique_ptr<ClientService> p(client);
	if(this->acceptor_.accept(client->peer())==-1) {
		ACE_ERROR_RETURN((LM_ERROR, "%p\n", "Failed to accept "), -1);
	}
	p.release();
	client->reactor(this->reactor());
	if(client->open()==-1) {
		client->handle_close(ACE_INVALID_HANDLE, 0);
	}
	return 0;
}

int ClientAcceptor::handle_close(ACE_HANDLE, ACE_Reactor_Mask) {
	if(this->acceptor_.get_handle()!=ACE_INVALID_HANDLE) {
		ACE_Reactor_Mask m=ACE_Event_Handler::ACCEPT_MASK|ACE_Event_Handler::DONT_CALL;
		this->reactor()->remove_handler(this, m);
		this->acceptor_.close();
	}
	return 0;
}

int ClientService::open(void) {
	char peer_name[MAXHOSTNAMELEN];
	ACE_INET_Addr peer_addr;
	if((this->sock_.get_remote_addr(peer_addr)==0) && (peer_addr.addr_to_string(peer_name, MAXHOSTNAMELEN)==0)) {
		ACE_DEBUG((LM_DEBUG, "(%P|%t) Connection from %s\n", peer_name));
	}
	return this->reactor()->register_handler (this, ACE_Event_Handler::READ_MASK);
}

int ClientService::handle_input(ACE_HANDLE) {
	const size_t INPUT_SIZE=4096;
	char buffer[INPUT_SIZE];
	ssize_t recv_cnt, send_cnt;
	if((recv_cnt=this->sock_.recv(buffer, sizeof(buffer)))<=0) {
		ACE_DEBUG((LM_DEBUG, "(%P|%t) Connection closed\n"));
		return -1;
	}
	send_cnt=this->sock_.send(buffer, static_cast<size_t>(recv_cnt));
	if(send_cnt==recv_cnt) {
		return 0;
	}
	if((send_cnt==-1) && (ACE_OS::last_error()!=EWOULDBLOCK)) {
		ACE_ERROR_RETURN((LM_ERROR, "(%P|%t) %p\n", "send"), 0);
	}
	if(send_cnt==-1) {
		send_cnt=0;
	}
	ACE_Message_Block* mb;
	size_t remaining=static_cast<size_t>((recv_cnt-send_cnt));
	ACE_NEW_RETURN(mb, ACE_Message_Block(remaining), -1);
	mb->copy(&buffer[send_cnt], remaining);
	int output_off=this->output_queue_.is_empty();
	ACE_Time_Value nowait(ACE_OS::gettimeofday());
	if(this->output_queue_.enqueue_tail(mb, &nowait)==-1) {
		ACE_ERROR((LM_ERROR, "(%P|%t) %p; discarding data\nenqueue failed"));
		mb->release();
		return 0;
	}
	if(output_off) {
		return this->reactor()->register_handler (this, ACE_Event_Handler::WRITE_MASK);
	}
	return 0;
}

int ClientService::handle_output(ACE_HANDLE) {
	ACE_Message_Block* mb;
	ACE_Time_Value nowait(ACE_OS::gettimeofday());
	while(0<=this->output_queue_.dequeue_head(mb, &nowait)) {
		ssize_t send_cnt=this->sock_.send(mb->rd_ptr(), mb->length());
		if(send_cnt==-1) {
			ACE_ERROR((LM_ERROR, "(%P|%t) %p\n", "send"));
		} else {
			mb->rd_ptr(static_cast<size_t>(send_cnt));
		}
		if(mb->length()>0) {
			this->output_queue_.enqueue_head(mb);
			break;
		}
		mb->release();
	}
	return (this->output_queue_.is_empty()) ? -1:0;
}

int ClientService::handle_close(ACE_HANDLE, ACE_Reactor_Mask mask) {
	if(mask==ACE_Event_Handler::WRITE_MASK) {
		return 0;
	}
	mask=ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL;
	this->reactor()->remove_handler(this, mask);
	this->sock_.close();
	this->output_queue_.flush();
	delete this;
	return 0;
}

class LoopStopper: public ACE_Event_Handler {
public:
	LoopStopper(int signum=SIGINT);
	// Called when object is signaled by OS.
	virtual int handle_signal(int signum, siginfo_t* =0, ucontext_t* =0);
};

LoopStopper::LoopStopper(int signum) {
	ACE_Reactor::instance()->register_handler(signum, this);
}

int LoopStopper::handle_signal(int, siginfo_t*, ucontext_t*) {
	ACE_Reactor::instance()->end_reactor_event_loop();
	return 0;
}

class LogSwitcher: public ACE_Event_Handler {
public:
	LogSwitcher(int on_sig, int off_sig);
	// Called when object is signaled by OS.
	virtual int handle_signal(int signum, siginfo_t* =0, ucontext_t* =0);
	// Called when an exceptional event occurs.
	virtual int handle_exception(ACE_HANDLE fd=ACE_INVALID_HANDLE);

private:
	LogSwitcher() {
	}
	// Signal to turn logging on
	int on_sig_;
	// Signal to turn logging off
	int off_sig_;
	// 1==turn on, 0==turn off
	int on_off_;
};

LogSwitcher::LogSwitcher(int on_sig, int off_sig) : on_sig_(on_sig), off_sig_(off_sig) {
	ACE_Sig_Set sigs;
	sigs.sig_add(on_sig);
	sigs.sig_add(off_sig);
	ACE_Reactor::instance()->register_handler(sigs, this);
	ACE_DEBUG((LM_DEBUG, "After register_handler\n"));
}

int LogSwitcher::handle_signal(int signum, siginfo_t *, ucontext_t *) {
	if((signum==this->on_sig_) || (signum==this->off_sig_)) {
		this->on_off_=signum==this->on_sig_;
		ACE_Reactor::instance()->notify(this);
		ACE_DEBUG((LM_DEBUG, "After notify\n"));
	}
	return 0;
}

int LogSwitcher::handle_exception(ACE_HANDLE) {
	if(this->on_off_) {
		ACE_LOG_MSG->clr_flags(ACE_Log_Msg::SILENT);
	} else {
		ACE_LOG_MSG->set_flags(ACE_Log_Msg::SILENT);
	}
	return 0;
}

int main() {
	ACE_INET_Addr port_to_listen("HAStatus");
	ClientAcceptor acceptor;
	acceptor.reactor(ACE_Reactor::instance());
	if(acceptor.open(port_to_listen)==-1) {
		return 1;
	}
	ACE_Reactor::instance()->run_reactor_event_loop();
	return EXIT_SUCCESS;
}
