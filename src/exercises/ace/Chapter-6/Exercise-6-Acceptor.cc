/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <ace/streams.h>
#include <ace/Log_Msg.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Stream.h>
#include <ace/Message_Queue.h>
#include <ace/OS_NS_unistd.h>
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE

/*
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 */

const int BUFFER_SIZE=1024;

class Server {
private:
	ACE_INET_Addr server_addr_;
	ACE_INET_Addr client_addr_;
	ACE_SOCK_Acceptor peer_acceptor_;
	ACE_SOCK_Stream new_stream_;

public:
	Server(int port) : server_addr_(port), peer_acceptor_(server_addr_) {
	}
	// Handle the connection once it has been established. Here the
	// connection is handled by reading BUFFER_SIZE amount of data from the
	// remote and then closing the connection stream down.
	int handle_connection() {
		// Read data from client
		static char message[BUFFER_SIZE + 1];
		// Save some more space than being read
		while(true) {
			int byte_count=0;
			if ((byte_count=new_stream_.recv(message, BUFFER_SIZE))==-1) {
				ACE_ERROR((LM_ERROR, "%p\n", "Error in recv"));
			} else {
				message[byte_count]=0;
				ACE_DEBUG((LM_DEBUG, "%s \n", message));
			}
			if (!ACE_OS::strcmp(message, "Quit")) {
				break;
			}
		}
// Close new endpoint
		if (new_stream_.close()==-1) {
			ACE_ERROR((LM_ERROR, "%p\n", "close"));
		}
		return(0);
	}

// Use the acceptor component peer_acceptor_ to accept the connection
// into the underlying stream new_stream_. After the connection has been
// established call the handle_connection() method.
	int accept_connections() {
		if (peer_acceptor_.get_local_addr(server_addr_)==-1) {
			ACE_ERROR_RETURN((LM_ERROR, "%p\n", "Error in get_local_addr"), 1);
		}
		ACE_DEBUG((LM_DEBUG, "Starting server at port %d\n", server_addr_.get_port_number()));
// Performs the iterative server activities.
		while(true) {
			ACE_Time_Value timeout(ACE_DEFAULT_TIMEOUT);
			if (peer_acceptor_.accept(new_stream_, &client_addr_, &timeout)==-1) {
				ACE_ERROR((LM_ERROR, "%p\n", "accept"));
				continue;
			} else {
				ACE_DEBUG((LM_DEBUG, "Connection established with remote %s:%d\n", client_addr_.get_host_name(), client_addr_.get_port_number()));
				// Handle the connection
				handle_connection();
				ACE_OS::exit(EXIT_FAILURE);
			}
		}
		return 0;
	}
};

int main(int argc, char** argv, char** envp) {
	if (argc < 2) {
		ACE_ERROR((LM_ERROR, "%s: usage %s <port_num>\n", argv[0], argv[0]));
		ACE_OS::exit(EXIT_FAILURE);
	}
	Server server(ACE_OS::atoi(argv[1]));
	server.accept_connections();
	return EXIT_SUCCESS;
}
