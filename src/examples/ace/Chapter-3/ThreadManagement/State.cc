/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <ace/Task.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

class HA_CommandHandler : public ACE_Task_Base {
public:
	virtual int svc(void) {
		ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) Handler Thread running\n")));
		return(0);
	}
};

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Main Thread running\n")));
	HA_CommandHandler handler;
	int result=handler.activate(THR_NEW_LWP | THR_JOINABLE | THR_SUSPENDED);
	ACE_ASSERT(result==0);
	ACE_UNUSED_ARG(result);
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) The current thread count is %d\n"), handler.thr_count()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) The group identifier is %d\n"), handler.grp_id()));
	handler.resume();
	handler.wait();
	return EXIT_SUCCESS;
}
