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
#include <ace/config-lite.h>
#include <ace/Synch.h>
#include <ace/Task.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

class HA_Device_Repository{
public:
	HA_Device_Repository() {
	}
	void update_device(int device_id) {
		mutex_.acquire();
		ACE_DEBUG((LM_DEBUG, "(%t) Updating device %d\n", device_id));
		ACE_OS::sleep(1);
		mutex_.release();
		// Let the other thread to aquire too
		ACE_OS::sleep(1);
	}

private:
	ACE_Thread_Mutex mutex_;
};
class HA_CommandHandler: public ACE_Task_Base {
public:
	enum {
		NUM_USES=10
	};
	HA_CommandHandler(HA_Device_Repository & rep) : rep_(rep) {
	}
	virtual int svc(void) {
		ACE_DEBUG((LM_DEBUG, "(%t) Handler Thread running\n"));
		for(int i=0; i < NUM_USES; i++) {
			this->rep_.update_device(i);
		}
		return 0;
	}

private:
	HA_Device_Repository& rep_;
};

int main() {
	HA_Device_Repository rep;
	HA_CommandHandler handler1(rep);
	HA_CommandHandler handler2(rep);
	handler1.activate();
	handler2.activate();
	handler1.wait();
	handler2.wait();
	return EXIT_SUCCESS;
}
