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
#define ACE_NTRACE 1
#include <ace/streams.h>
#include <ace/Log_Msg.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

void FunB() {
	ACE_TRACE("FunB");
	ACE_DEBUG((LM_DEBUG, "DEBUG: pid=%P Line:%l File=%N\n"));
	ACE_DEBUG((LM_INFO, "INFO: pid=%P Line:%l File=%N\n"));
	ACE_DEBUG((LM_NOTICE, "NOTICE: pid=%P Line:%l File=%N\n"));
}

void FunA() {
	ACE_TRACE("FunA");
	ACE_DEBUG((LM_DEBUG, "DEBUG: pid=%P Line:%l File=%N\n"));
	ACE_DEBUG((LM_INFO, "INFO: pid=%P Line:%l File=%N\n"));
	ACE_DEBUG((LM_NOTICE, "NOTICE: pid=%P Line:%l File=%N\n"));
	FunB();
}

int main(int argc __attribute__((unused)), char** argv) {
	// Set a value just initializing the while loop. It will terminate when EOF will generate type=0
	ACE_LOG_MSG->open(argv[0], ACE_Log_Msg::SYSLOG, "ACE-Course");
	ACE_DEBUG((LM_INFO, "%I a - This was here\n"));
	ACE_DEBUG((LM_DEBUG, "%I b - This Can't be here\n"));
	ACE_DEBUG((LM_INFO, "argv[0]=%s\n", argv[0]));
	ACE_DEBUG((LM_INFO, "INFO: pid=%P Line:%l File=%N\n"));
	ACE_LOG_MSG->priority_mask(LM_DEBUG | LM_INFO | LM_NOTICE | LM_TRACE, ACE_Log_Msg::PROCESS);
	ACE_TRACE("main");
	for(int i=0; i<2; i++) {
		FunA();
	}
	ACE_LOG_MSG->open(argv[0]);
	ACE_DEBUG((LM_INFO, "End of Program\n"));
	return EXIT_SUCCESS;
}
