/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
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
#define ACE_NTRACE 1
#include <ace/OS_NS_stdio.h>
#include <ace/Malloc_Base.h>
#include <ace/Message_Queue.h>
#include <ace/Read_Buffer.h>
#include <ace/Thread_Manager.h>
#include <ace/Service_Config.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 * EXTRA_LINK_FLAGS=-lACE -ldl -lrt -lpthread
 */

// Global thread manager.
static ACE_Thread_Manager thr_mgr;
// Make the queue be capable of being *very* large.
static const long max_queue=LONG_MAX;

ACE_Message_Queue<ACE_MT_SYNCH> msg_queue1(max_queue);

ACE_Message_Queue<ACE_MT_SYNCH> msg_queue2(max_queue);

// The consumer dequeues a message from the ACE_Message_Queue, writes
// the message to the stderr stream, and deletes the message. The
// producer sends a 0-sized message to inform the consumer to stop
// reading and exit.
static void *consumer(ACE_Message_Queue<ACE_MT_SYNCH> *msg_queue) {
	// Keep looping, reading a message out of the queue, until we
	// timeout or get a message with a length==0, which signals us to quit.
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("consumer: thread=%t Line:%l\n")));
	ACE_OS::sleep(1);
	while(true) {
		ACE_Message_Block *mb;
		if (msg_queue->dequeue_head(mb)==-1) {
			break;
		}
		size_t length=mb->length();
		if (mb->msg_type()==5) {
			ACE_DEBUG((LM_DEBUG, "Special Message: <%s> type: %d\n", mb->rd_ptr() + 2, mb->msg_type()));
			// Change the message type 4 and re-send
			mb->msg_type(4);
			if (msg_queue2.enqueue_head(mb)==-1) {
				// Re-send the message !!
				ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
			}
		} else {
			if (length > 0) {
				ACE_DEBUG((LM_DEBUG, "------- Message: <%s> type: %d\n", mb->rd_ptr() + 2, mb->msg_type()));
			}
			// Free up the buffer memory and the Message_Block.
			// ACE_Allocator::instance ()->free (mb->rd_ptr ()); // Free the buffer
			mb->release();
			// Free the Memory Block
			if (length==0) {
				break;
			}
		}
	}
	return(0);
}

// The producer reads data from the stdin stream, creates a message,
// and then queues the message in the message list, where it is
// removed by the consumer thread. A 0-sized message is enqueued when
// there is no more data to read. The consumer uses this as a flag to
// know when to exit.
static void *producer(void*) {
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("producer: thread=%t Line:%l\n")));
	ACE_Read_Buffer rb(ACE_STDIN);
	// Keep reading stdin, until we reach EOF.
	while(true) {
		// Allocate a new buffer.
		char* buffer=rb.read('\n');
		ACE_Message_Block *mb;
		if (buffer==0) {
			// Send a 0-sized shutdown message to the other thread and exit.
			ACE_NEW_RETURN(mb, ACE_Message_Block((size_t)0), 0);
			// Send Zero size message to both queues !!!
			if (msg_queue1.enqueue_tail(mb)==-1) {
				ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
			}
			ACE_NEW_RETURN(mb, ACE_Message_Block((size_t)0), 0);
			if (msg_queue2.enqueue_tail(mb)==-1) {
				ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
			}
			break;
		} else {
			// Enqueue the message in priority order.
			// Allocate a new message, but have it "borrow" its memory from the buffer.
			ACE_NEW_RETURN(mb, ACE_Message_Block(rb.size(), ACE_Message_Block::MB_DATA, 0, buffer), 0);
			// get message size
			mb->wr_ptr(rb.size());
			// ACE_DEBUG ((LM_DEBUG, "enqueueing message of size %d\n", size));
			// Get message type into c variable
			char c=*buffer;
			switch (c) {
			case '1':
				ACE_DEBUG((LM_DEBUG, "case1: <%c>\n", c));
				// Set message type
				mb->msg_type(1);
				// Enqueue in tail queue1
				if (msg_queue1.enqueue_tail(mb)==-1) {
					ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
				}
				break;

			case '2':
				ACE_DEBUG((LM_DEBUG, "case2: <%c>\n", c));
				// Set message type
				mb->msg_type(2);
				// Enqueue in head queue1
				if (msg_queue1.enqueue_head(mb)==-1) {
					ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
				}
				break;

			case '3':
				ACE_DEBUG((LM_DEBUG, "case3: <%c>\n", c));
				// Set message type
				mb->msg_type(3);
				// Enqueue in tail queue2
				if (msg_queue2.enqueue_tail(mb)==-1) {
					ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
				}
				break;

			case '4':
				ACE_DEBUG((LM_DEBUG, "case4: <%c>\n", c));
				// Set message type
				mb->msg_type(4);
				// Enqueue in head queue2
				if (msg_queue2.enqueue_head(mb)==-1) {
					ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
				}
				break;

			default:
				ACE_DEBUG((LM_DEBUG, "default: <%c> doing nothing!!!\n", c));
				// Set message type
				mb->msg_type(5);
				mb->msg_priority(10);
				if (msg_queue1.enqueue_prio(mb)==-1) {
					ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
				}
				break;
			}
		}
		// ACE_OS::sleep (1);
	}
	// finished ? then return
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("end of producer: thread=%t Line:%l\n")));
	return(0);
}

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	// Spawn off one thread that copies stdin to stdout in order of the size of each line.
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("main: thread=%t Line:%l\n")));
	if (thr_mgr.spawn(ACE_THR_FUNC(producer), (void *)NULL, THR_NEW_LWP | THR_DETACHED)==-1) {
		ACE_ERROR_RETURN((LM_ERROR, "%p\n", "spawn producer"), 1);
	}
	if (thr_mgr.spawn(ACE_THR_FUNC(consumer), (void *)&msg_queue1, THR_NEW_LWP | THR_DETACHED)==-1) {
		ACE_ERROR_RETURN((LM_ERROR, "%p\n", "spawn consumer1"), 1);
	}
	if (thr_mgr.spawn(ACE_THR_FUNC(consumer), (void *)&msg_queue2, THR_NEW_LWP | THR_DETACHED)==-1) {
		ACE_ERROR_RETURN((LM_ERROR, "%p\n", "spawn consumer2"), 1);
	}
	// Wait for producer and consumers threads to exit.
	thr_mgr.wait();
	return EXIT_SUCCESS;
}
