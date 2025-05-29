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
#define ACE_NTRACE 0
#include <ace/Log_Msg.h>
#include <ace/Process_Manager.h>
#include <ace/Reactor.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

static const int NCHILDREN=2;

class DeathHandler: public ACE_Event_Handler {
public:
	DeathHandler() : count_(0) {
		ACE_TRACE("DeathHandler::DeathHandler");
	}
	virtual int handle_exit(ACE_Process *process) {
		ACE_TRACE("DeathHandler::handle_exit");
		ACE_DEBUG ((LM_DEBUG, "Process %d exited with exit code %d\n", process->getpid(), process->return_value()));
		if(++count_==NCHILDREN) {
			ACE_Reactor::instance()->end_reactor_event_loop();
		}
		return 0;
	}

private:
	int count_;
};

int main(int argc, char** argv) {
	if(argc>1) {
		// Running as a child.
		return 0;
	}
	// Instantiate a process manager with space for 10 processes.
	ACE_Process_Manager pm(10, ACE_Reactor::instance());
	// Create a process termination handler.
	DeathHandler handler;
	// Specify the options for the new processes to be spawned.
	ACE_Process_Options options;
	options.command_line("%s a", argv[0]);
	// Spawn two child processes.
	pid_t pids[NCHILDREN];
	pm.spawn_n(NCHILDREN, options, pids);
	// Register handler to be called when these processes exit.
	for(int i=0; i<NCHILDREN; i++) {
		pm.register_handler(&handler, pids[i]);
	}
	// Run the reactor event loop waiting for events to occur.
	ACE_Reactor::instance()->run_reactor_event_loop();
	return EXIT_SUCCESS;
}
