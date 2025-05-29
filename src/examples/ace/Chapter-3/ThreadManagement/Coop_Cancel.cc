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
#include <ace/OS_NS_time.h>
#include <ace/OS_NS_unistd.h>
#include <ace/Task.h>
#include <ace/Log_Msg.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 * EXTRA_LINK_FLAGS_AFTER=-lACE -ldl -lrt -lpthread
 */

class CanceledTask: public ACE_Task<ACE_MT_SYNCH> {
public:

	virtual int svc(void) {
		ACE_DEBUG((LM_DEBUG, "(%t) starting up \n"));

		// Cache our ACE_Thread_Manager pointer.
		ACE_Thread_Manager *mgr=this->thr_mgr();
		while(true) {
			if (mgr->testcancel(mgr->thr_self())) {
				return 0;
			}
			ACE_Message_Block *mb=0;
			ACE_Time_Value tv(0, 1000);

			tv+=ACE_OS::time(0);
			int result=this->getq(mb, &tv);
			if ((result==-1) && (errno==EWOULDBLOCK)) {
				continue;
			} else {
				// Do real work.
			}
		}
		ACE_NOTREACHED(return 0);
	}
};

int main() {
	CanceledTask task;
	task.activate();
	ACE_OS::sleep(1);
	ACE_Thread_Manager::instance()->cancel_task(&task);
	task.wait();
	return EXIT_SUCCESS;
}
