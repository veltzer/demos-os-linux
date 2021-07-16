/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <ace/Timer_Queue.h>
#include <ace/Reactor.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 * EXTRA_LINK_FLAGS=-lACE -ldl -lrt -lpthread
 */

#define Message_Offset 6
// Global thread manager.
static ACE_Thread_Manager thr_mgr;
// Make the queue be capable of being *very* large.
static const long max_queue=LONG_MAX;

ACE_Message_Queue<ACE_MT_SYNCH> msg_queue1(max_queue);

ACE_Message_Queue<ACE_MT_SYNCH> msg_queue2(max_queue);
static void *consumer(ACE_Message_Queue<ACE_MT_SYNCH> *msg_queue);

ACE_Reactor reactor;	// Reactor

static int done=0;	// Termination state of both consumers

class MyTime_Handler : public ACE_Event_Handler {
public:
	// Method which is called back by the Reactor when timeout occurs.
	virtual int handle_timeout(const ACE_Time_Value& tv, const void *arg) {
		long argument=long(arg);
		// ACE_DEBUG ((LM_DEBUG, "Timer timed out at %d! arg=%d\n", tv.sec(), argument));
		// Activate the consumer
		if (argument==1) {
			if (thr_mgr.spawn(ACE_THR_FUNC(consumer), (void *)&msg_queue1, THR_NEW_LWP | THR_DETACHED)==-1) {
				ACE_ERROR_RETURN((LM_ERROR, "%p\n", "spawn consumer1"), 1);
			}
			return(1);
		} else {
			if (thr_mgr.spawn(ACE_THR_FUNC(consumer), (void *)&msg_queue2, THR_NEW_LWP | THR_DETACHED)==-1) {
				ACE_ERROR_RETURN((LM_ERROR, "%p\n", "spawn consumer2"), 1);
			}
			return(1);
		}
		// Keep yourself registered with the Reactor.
		return(0);
	}
};

// The consumer dequeues a message from the ACE_Message_Queue, writes
// the message to the stderr stream, and deletes the message. The
// producer sends a 0-sized message to inform the consumer to stop
// reading and exit.
static void *consumer(ACE_Message_Queue<ACE_MT_SYNCH> *msg_queue) {
	// Keep looping, reading a message out of the queue, until we
	// timeout or get a message with a length==0, which signals us to quit.

	// ACE_DEBUG ((LM_DEBUG , ACE_TEXT ("consumer: thread=%t Line:%l\n")));
	// ACE_OS::sleep (1);
	// Read ONLY one message and return !!!
	ACE_Message_Block *mb;
	if (msg_queue->dequeue_head(mb)==-1) {
		return(0);
	}
	size_t length=mb->length();
	// Skip the type and delay
	mb->rd_ptr(mb->rd_ptr() + Message_Offset);
	if (length > 0) {
		ACE_OS::puts((mb->rd_ptr()));
	}
	// Free up the buffer memory and the Message_Block.
	// Return to the actual buffer address
	mb->rd_ptr(mb->rd_ptr() - Message_Offset);
	// ACE_Allocator::instance ()->free (mb->rd_ptr ());
	// Free the buffer
	mb->release();
	// End of data !!!
	if (!length) {
		done++;
	}
	return(0);
}

// The producer reads data from the stdin stream, creates a message,
// and then queues the message in the message list, where it is
// removed by the consumer thread. A 0-sized message is enqueued when
// there is no more data to read. The consumer uses this as a flag to
// know when to exit.
static void *producer(void*) {
	MyTime_Handler *th=new MyTime_Handler;

	ACE_DEBUG((LM_DEBUG, ACE_TEXT("producer: thread=%t Line:%l\n")));
	ACE_Read_Buffer rb(ACE_STDIN);
	// Keep reading stdin, until we reach EOF.
	while(true) {
		// Allocate a new buffer.
		char* buffer=rb.read('\n');
		ACE_Message_Block *mb;
		int timer_id;
		int type;
		float delay;
		static float AbsoluteDelay=0.0;
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
			// Specify queue1 in handle_timeout()
			reactor.schedule_timer(th, (const void *)1, ACE_Time_Value(AbsoluteDelay + 0.1));
			// Specify queue2 in handle_timeout()
			reactor.schedule_timer(th, (const void *)2, ACE_Time_Value(AbsoluteDelay + 0.2));
			break;
		} else {
			// Enqueue the message in priority order.
			sscanf(buffer, "%d %f", &type, &delay);
			AbsoluteDelay+=delay;
			// Allocate a new message, but have it "borrow" its memory from the buffer.
			ACE_NEW_RETURN(mb, ACE_Message_Block(rb.size(), ACE_Message_Block::MB_DATA, 0, buffer), 0);
			// get message size
			mb->wr_ptr(rb.size());
			// mb->rd_ptr(mb->rd_ptr() + Message_Offset); // Skip thhe type and time in the message
			// ACE_DEBUG ((LM_DEBUG, "enqueueing message of size %d\n", size));
			switch (type) {
			case 1:
				// Enqueue in tail queue1
				if (msg_queue1.enqueue_tail(mb)==-1) {
					ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
				}
				// Specify queue1 in handle_timeout()
				timer_id=reactor.schedule_timer(th, (const void *)1, ACE_Time_Value(AbsoluteDelay));
				// ACE_DEBUG ((LM_DEBUG , "case1: timer id=%d\n", timer_id));
				break;

			case 2:
				// Enqueue in head queue1
				if (msg_queue1.enqueue_head(mb)==-1) {
					ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
				}
				// Specify queue1 in handle_timeout()
				timer_id=reactor.schedule_timer(th, (const void *)1, ACE_Time_Value(AbsoluteDelay));
				// ACE_DEBUG ((LM_DEBUG , "case2: timer id=%d\n", timer_id));
				break;

			case 3:
				// Enqueue in tail queue2
				if (msg_queue2.enqueue_tail(mb)==-1) {
					ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
				}
				// Specify queue2 in handle_timeout()
				timer_id=reactor.schedule_timer(th, (const void *)2, ACE_Time_Value(AbsoluteDelay));
				// ACE_DEBUG ((LM_DEBUG , "case3: timer id=%d\n", timer_id));
				break;

			case 4:
				// Enqueue in head queue2
				if (msg_queue2.enqueue_head(mb)==-1) {
					ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
				}
				// Specify queue2 in handle_timeout()
				timer_id=reactor.schedule_timer(th, (const void *)2, ACE_Time_Value(AbsoluteDelay));
				ACE_DEBUG((LM_DEBUG, "case4: timer id=%d\n", timer_id));
				break;

			default:
				ACE_DEBUG((LM_DEBUG, "default: <%d> doing nothing!!!\n", type));
				break;
			}
		}
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
	// Wait for producer thread to exit. Comsumer thread is handled by the done variable
	thr_mgr.wait();
	// wait for all events to be completed
	while(done!=2) {
		reactor.handle_events();
	}
	return EXIT_SUCCESS;
}
