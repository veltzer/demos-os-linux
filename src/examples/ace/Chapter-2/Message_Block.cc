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
#include <ace/OS_Memory.h>	// for ACE_NEW_RETURN
#include <ace/OS_NS_stdio.h>	// This is for snprintf
#include <ace/Log_Msg.h>// This is for ACE_DEBUG
#include <ace/Message_Block.h>	// This is for ACE_Message_Block
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE

/*
 * This demo shows how to create message blocks, how to set message types, how to create
 * error or hangup message, how to populate message blocks in static or dynamic fashion
 *
 * This demo does not explain what ACE_NEW_RETURN is. See slides or a different example
 * for that...
 *
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	// This sections explains how to create a message block with some data in it
	// It shows how to use the ACE_NEW_RETURN construct
	// How to initialize a message block with constant size and how to write
	// data to the message block
	ACE_Message_Block *mb;
	const int size=128;

	// Notice that we define a constant size block
	ACE_NEW_RETURN(mb, ACE_Message_Block(size), EXIT_FAILURE);
	const char *data1="This is some data.";
	const char *data2="This is even more Data...";
	// We have a 128 bytes at mb->wr_ptr so lets write something on them
	// mb->wr_ptr also moves the pointer forward
	mb->wr_ptr(ACE_OS::snprintf(mb->wr_ptr(), size, "%s", data1));
	mb->wr_ptr(ACE_OS::snprintf(mb->wr_ptr(), size-strlen(data1), "%s", data2));
	// Lets access the buffer in read only mode
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Message data --> %C\n"), mb->rd_ptr()));
	// This is to show that the rd_ptr does not change across calls
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Message data --> %C\n"), mb->rd_ptr()));
	// This is to show that we can set the read pointer to some place
	// and read from there
	// lets do it using a pointer (absolutely)
	mb->rd_ptr(mb->rd_ptr() + 6);
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Message data --> %C\n"), mb->rd_ptr()));
	// lets do it using an integer (relativly)
	mb->rd_ptr(-2);
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Message data --> %C\n"), mb->rd_ptr()));
	// again, rd_ptr does not change
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Message data --> %C\n"), mb->rd_ptr()));
	// Lets print the messages type. This is the default one
	// Lets also print some more types to compare
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Messages type is --> %d\n"), mb->msg_type()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("MB_HANGUP is --> %d\n"), ACE_Message_Block::MB_HANGUP));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("MB_ERROR is --> %d\n"), ACE_Message_Block::MB_ERROR));
	// Lets set the message blocks type to something and show it
	mb->msg_type(7);
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Messages type is --> %d\n"), mb->msg_type()));
	// Lets print the message size and the message length
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("size is --> %d\n"), mb->size()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("length is --> %d\n"), mb->length()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("total_size is --> %d\n"), mb->total_size()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("total_length is --> %d\n"), mb->total_length()));
	// Lets show how the dump method work (does not do anything by default)
	mb->dump();

	// finally, lets get ridd of the message block
	mb->release();

	// Send a hangup notification to the receiver. Notice the type parameter
	ACE_NEW_RETURN(mb, ACE_Message_Block(size, ACE_Message_Block::MB_HANGUP), EXIT_FAILURE);
	// Lets print the Message_Block's types
	if (mb->msg_type()==ACE_Message_Block::MB_HANGUP) {
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("We got a hangup message\n")));
	}
	// again, release the mb
	mb->release();

	// Send an error notification to the receiver. Notice the type parameter
	ACE_NEW_RETURN(mb, ACE_Message_Block(size, ACE_Message_Block::MB_ERROR), EXIT_FAILURE);
	if (mb->msg_type()==ACE_Message_Block::MB_ERROR) {
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("We got an error message\n")));
	}
	// again, release the mb
	mb->release();

	// Create a non constant size Message_Block...
	ACE_NEW_RETURN(mb, ACE_Message_Block(), EXIT_FAILURE);
	// Lets allocate message block size and write some data
	mb->size(size);
	mb->wr_ptr(ACE_OS::snprintf(mb->wr_ptr(), size, "%s", data1));
	// Lets print the message size and the message length
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("size is --> %d\n"), mb->size()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("length is --> %d\n"), mb->length()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("total_size is --> %d\n"), mb->total_size()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("total_length is --> %d\n"), mb->total_length()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Message data --> %C\n"), mb->rd_ptr()));
	// Lets re-allocate more space and print again
	mb->size(size * 2);
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("size is --> %d\n"), mb->size()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("length is --> %d\n"), mb->length()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("total_size is --> %d\n"), mb->total_size()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("total_length is --> %d\n"), mb->total_length()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Message data --> %C\n"), mb->rd_ptr()));

	// Lets start playing around with reference counting
	ACE_NEW_RETURN(mb, ACE_Message_Block(size), EXIT_FAILURE);
	mb->wr_ptr(ACE_OS::snprintf(mb->wr_ptr(), size, "%s", data1));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("reference_count is --> %d\n"), mb->reference_count()));
	ACE_Message_Block* mb2=mb->duplicate();
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("reference_count is --> %d\n"), mb->reference_count()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("reference_count is --> %d\n"), mb2->reference_count()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Message data --> %C\n"), mb->rd_ptr()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Message data --> %C\n"), mb2->rd_ptr()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Read pointer --> %x\n"), mb->rd_ptr()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Read pointer --> %x\n"), mb2->rd_ptr()));

	// lets advance one of the read pointers (we can
	// see that each has it's own read pointer...)
	mb->rd_ptr(3);
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Message data --> %C\n"), mb->rd_ptr()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Message data --> %C\n"), mb2->rd_ptr()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Read pointer --> %x\n"), mb->rd_ptr()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Read pointer --> %x\n"), mb2->rd_ptr()));

	mb->release();
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("reference_count is --> %d\n"), mb2->reference_count()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Message data --> %C\n"), mb2->rd_ptr()));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Read pointer --> %x\n"), mb2->rd_ptr()));
	return EXIT_SUCCESS;
}
