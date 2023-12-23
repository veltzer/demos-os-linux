/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <ace/OS_NS_stdio.h>
#include <ace/Malloc_Base.h>	// To get ACE_Allocator
#include <ace/Message_Queue.h>
#include <ace/Read_Buffer.h>
#include <ace/Thread_Manager.h>
#include <ace/Service_Config.h>
#include <ace/Truncate.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * Exercise 2
 * Please note this is NOT a running prgram.
 * The consumer is activated by the producer (see line 85)
 * According to the Exercise there should be two consumers.
 * Hint: DO NOT ACTIVATE them from the producer. !!!
 *
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 * EXTRA_LINK_FLAGS_AFTER=-lACE -ldl -lrt -lpthread
 */

// Global thread manager.
static ACE_Thread_Manager thr_mgr;
// Make the queue be capable of being *very* large.
static const long max_queue=LONG_MAX;

// The consumer dequeues a message from the ACE_Message_Queue
static void* consumer(ACE_Message_Queue<ACE_MT_SYNCH> *msg_queue) {
	ACE_Message_Block* mb;
	if (msg_queue->dequeue_head(mb)==-1) {
		return NULL;
	}
	int length=ACE_Utils::truncate_cast<int> (mb->length());
	if (length>0) {
		// Output to the STDOUT
		ACE_OS::puts(mb->rd_ptr());
	}
	// Free up the buffer memory and the Message_Block.
	ACE_Allocator::instance()->free(mb->rd_ptr());
	// Release the Message Block
	mb->release();
	if (length==0) {
		// you may use the zero length as program termination
		return NULL;
	}
	return(0);
}

// The producer reads data from the stdin stream, creates a message,
// and then queues the message in the message list, where it is
// removed by the consumer thread. A 0-sized message is enqueued when
// there is no more data to read. The consumer uses this as a flag to
// know when to exit.
static void* producer(ACE_Message_Queue<ACE_MT_SYNCH> *msg_queue) {
	// Create linkage to STDIN
	ACE_Read_Buffer rb(ACE_STDIN);
	// Keep reading stdin, until we reach EOF. (size is zero)
	// reading a line
	char* buffer=rb.read('\n');
	// Message Block definition
	ACE_Message_Block *mb;
	// Allocation of message block example
	// #define ACE_NEW_RETURN(POINTER,CONSTRUCTOR,RET_VAL)
	// and for the Message_Block as follows:
	//
	// ===ACE_Message_Block======From: <ace/Message_Block.h>===========================
	/*
	 * ACE_Message_Block (size_t size,
	 * ACE_Message_Block::ACE_Message_Type type=MB_DATA,
	 * ACE_Message_Block *cont=0,
	 * const char *data=0, // Pointer to the data buffer
	 * ACE_Allocator *allocator_strategy=0,
	 * ACE_Lock *locking_strategy=0,
	 * unsigned long priority=ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY,
	 * const ACE_Time_Value &execution_time=ACE_Time_Value::zero,
	 * const ACE_Time_Value &deadline_time=ACE_Time_Value::max_time,
	 * ACE_Allocator *data_block_allocator=0,
	 * ACE_Allocator *message_block_allocator=0);
	 */
	// ==============================================
	ACE_NEW_RETURN(mb, ACE_Message_Block(rb.size(), ACE_Message_Block::MB_DATA, 0, buffer), 0);
	// The following are methods for the queue (assuming using msg_queue):
	// mb->msg_priority (int); // Priority MUST be set into the message block
	// msg_queue->enqueue_head(mb)
	// msg_queue->enqueue_tail(mb)
	// msg_queue->enqueue_prio(mb)
	// all the above methods return -1 when error occures
	consumer(msg_queue);
	return(0);
}

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	// Spawn off one thread that copies stdin to stdout in order of the size of each line.
	ACE_Message_Queue<ACE_MT_SYNCH> msg_queue(max_queue);
	if(thr_mgr.spawn(ACE_THR_FUNC(producer), (void *)&msg_queue, THR_NEW_LWP|THR_DETACHED)==-1) {
		ACE_ERROR_RETURN((LM_ERROR, "%p\n", "spawn"), 1);
	}
	// Wait for producer and consumer threads to exit.
	thr_mgr.wait();
	return EXIT_SUCCESS;
}
