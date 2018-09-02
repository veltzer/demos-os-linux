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
#define ACE_NTRACE 0
#include <ace/OS_NS_unistd.h>
#include <ace/Log_Msg.h>
#include <ace/Process_Manager.h>

/*
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 */

static const int NCHILDREN=2;

int ACE_TMAIN(int argc, ACE_TCHAR *argv[]) {
	if (argc > 1) {
		// Running as a child.
		ACE_OS::sleep(10);
	} else {
		// Running as a parent.
		// Get the processwide process manager.
		ACE_Process_Manager *pm=ACE_Process_Manager::instance();

		// Specify the options for the new processes
		// to be spawned.
		ACE_Process_Options options;
		options.command_line(ACE_TEXT("%s a"), argv[0]);

		// Spawn two child processes.
		pid_t pids[NCHILDREN];
		pm->spawn_n(NCHILDREN, options, pids);

		// Destroy the first child.
		pm->terminate(pids[0]);

		// Wait for the child we just terminated.
		ACE_exitcode status;
		pm->wait(pids[0], &status);

		// Get the results of the termination.

#if !defined(ACE_WIN32)
		if (WIFSIGNALED(status)!=0) {
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("%d died because of a signal of type %d\n"), pids[0], WTERMSIG(status)));
		}
#else
		ACE_DEBUG ((LM_DEBUG, ACE_TEXT("The process terminated with exit code %d\n"), status));
#endif	/*ACE_WIN32*/

		// Wait for all (only one left) of the
		// children to exit.
		pm->wait(0);
	}
	return(0);
}
