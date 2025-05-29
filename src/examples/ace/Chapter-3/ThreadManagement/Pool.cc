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
#include <ace/Task.h>
#include <ace/Log_Msg.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 * EXTRA_LINK_FLAGS_AFTER=-lACE -ldl -lrt -lpthread
 */

class HA_CommandHandler: public ACE_Task<ACE_MT_SYNCH> {
public:
	virtual int svc(void) {
		ACE_DEBUG((LM_DEBUG, "(%t) starting up\n"));
		ACE_Message_Block* mb;
		if(this->getq(mb)==-1) {
			return -1;
		}
		// ... do something with the message.
		return 0;
	}
};

int main() {
	HA_CommandHandler handler;
	// Create 4 threads.
	handler.activate(THR_NEW_LWP | THR_JOINABLE, 4);
	handler.wait();
	return EXIT_SUCCESS;
}
