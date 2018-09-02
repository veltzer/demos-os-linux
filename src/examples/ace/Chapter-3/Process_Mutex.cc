/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <ace/OS_NS_unistd.h>
#include <ace/Log_Msg.h>
#include <ace/Process.h>
#include <ace/Process_Mutex.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 */

class GResourceUser {
public:
	GResourceUser(ACE_Process_Mutex & mutex) : gmutex_(mutex) {
		ACE_TRACE(ACE_TEXT("GResourceUser::GResourceUser"));
	}
	void run(void) {
		ACE_TRACE(ACE_TEXT("GResourceUser::run"));
		int count=0;
		while(count++<5) {
			int result=this->gmutex_.acquire();
			ACE_ASSERT(result==0);
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P| %t) has the mutex (count=%d)\n"), count));
			// Access Global resource
			ACE_OS::sleep(1);
			result=this->gmutex_.release();
			ACE_ASSERT(result==0);
			// Give other process a chance.
			ACE_OS::sleep(1);
		}
	}

private:
	ACE_Process_Mutex& gmutex_;
};

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	// Run as the child.
	if (argc > 1) {
		// Create or get the global mutex.
		ACE_Process_Mutex mutex("GlobalMutex");

		GResourceUser acquirer(mutex);

		acquirer.run();
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P| %t) Done (Child).\n")));
	} else {
		// Run as the parent.
		ACE_Process_Options options;
		options.command_line(ACE_TEXT("%s a"), argv[0]);
		ACE_Process processa, processb;

		pid_t pida=processa.spawn(options);
		pid_t pidb=processb.spawn(options);

		ACE_DEBUG((LM_DEBUG, ACE_TEXT("Spawned processes; pids %d:%d\n"), pida, pidb));
		if (processa.wait()==-1) {
			ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("processa wait")), -1);
		}
		if (processb.wait()==-1) {
			ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("processb wait")), -1);
		}
		// Print Done for the parent only
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%P| %t) Done (Parent).\n")));
	}
	return EXIT_SUCCESS;
}
