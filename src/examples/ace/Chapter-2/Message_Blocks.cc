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
#include <ace/OS_main.h>
#include <ace/OS_Memory.h>
#include <ace/OS_NS_stdio.h>
#include <ace/OS_NS_string.h>
#include <ace/Log_Msg.h>
#include <ace/Message_Block.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 */

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	ACE_Message_Block* mb;
	ACE_NEW_RETURN(mb, ACE_Message_Block(128), -1);
	const char *command1="<InitialDataPart>";
	const char *command2="< More Data ...>";
	// Use the pointer to insert the string
	ACE_OS::sprintf(mb->wr_ptr(), "%s", command1);
	// Move the wr_ptr() forward in the buffer by the amount of data we just put in.
	mb->wr_ptr(ACE_OS::strlen(command1));
	// Use the pointer to add another string
	ACE_OS::sprintf(mb->wr_ptr(), "%s", command2);
	mb->wr_ptr(ACE_OS::strlen(command2) + 1);

	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Command Sequence --> %C\n"), mb->rd_ptr()));
	mb->rd_ptr(ACE_OS::strlen(mb->rd_ptr()) + 1);
	mb->release();
	// Send a hangup notification to the receiver.
	ACE_NEW_RETURN(mb, ACE_Message_Block(128, ACE_Message_Block::MB_HANGUP), -1);
	// Send an error notification to the receiver.
	mb->msg_type(ACE_Message_Block::MB_ERROR);
	mb->release();
	return EXIT_SUCCESS;
}
