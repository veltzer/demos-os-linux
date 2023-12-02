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
#include <ace/OS_NS_stdio.h>	// for ACE_STDIN
#include <ace/Message_Queue.h>	// for the two message queues
#include <ace/Read_Buffer.h>	// for the read buffer
#include <ace/Thread_Manager.h>	// for the thread manager to manage everything
#include <stdlib.h>	// for EXIT_SUCCESS
#include <pthread_utils.h>	// for gettid()

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 * EXTRA_LINK_FLAGS_AFTER=-lACE -ldl -lrt -lpthread
 *
 * TODO:
 *
 */

// The consumer dequeues a message from the ACE_Message_Queue, writes
// the message to the stderr stream, and deletes the message. The
// producer sends a 0-sized message to inform the consumer to stop
// reading and exit.

typedef struct {
	int id;
	ACE_Message_Queue<ACE_MT_SYNCH>* msg_queue;
} ConsumerData;

static void *consumer(ConsumerData* pdata) {
	// Keep looping, reading a message out of the queue, until we
	// timeout or get a message with a length==0, which signals us to quit.
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) consumer %d starting\n"), pdata->id));
	printf("my tid is %d\n", gettid());
	while(true) {
		ACE_Message_Block *mb;
		if (pdata->msg_queue->dequeue_head(mb)==-1) {
			ACE_ERROR((LM_ERROR, ACE_TEXT("(%t) %d dequeue_head\n"), pdata->id));
			break;
		}
		if(mb->msg_type()==ACE_Message_Block::MB_HANGUP) {
			ACE_DEBUG((LM_ERROR, ACE_TEXT("(%t) consumer %d got hangup\n"), pdata->id));
			mb->release();
			break;
		} else {
			// the +1 is to avoid the type
			ACE_DEBUG((LM_ERROR, "(%t) consumer %d got message %s with size %d and length %d\n", pdata->id, mb->rd_ptr()+1, mb->size(), mb->length()));
			ACE_Allocator::instance()->free(mb->rd_ptr());
			mb->release();
		}
	}
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) consumer %d ending\n"), pdata->id));
	return NULL;
}

// The producer reads data from the stdin stream, creates a message,
// and then queues the message in the message list, where it is
// removed by the consumer thread. A 0-sized message is enqueued when
// there is no more data to read. The consumer uses this as a flag to
// know when to exit.

typedef struct {
	ACE_Message_Queue<ACE_MT_SYNCH>* msg_queue1;
	ACE_Message_Queue<ACE_MT_SYNCH>* msg_queue2;
} ProducerData;

static void *producer(ProducerData* pdata) {
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) producer starting\n")));
	printf("my tid is %d\n", gettid());
	ACE_Read_Buffer rb(ACE_STDIN);
	// Keep reading stdin, until we reach EOF.
	while(true) {
		// Allocate a new buffer.
		char* buffer=rb.read('\n');
		if (buffer==NULL) {
			// create a special hangup kmessage
			ACE_DEBUG((LM_ERROR, "(%t) producer sending hangup message\n"));
			ACE_Message_Block *mb1, *mb2;
			ACE_NEW_RETURN(mb1, ACE_Message_Block(0, ACE_Message_Block::MB_HANGUP), NULL);
			mb2=mb1->duplicate();
			// send the special message to both of the threads
			if(pdata->msg_queue1->enqueue_tail(mb1)==-1) {
				ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
			}
			if(pdata->msg_queue2->enqueue_tail(mb2)==-1) {
				ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
			}
			break;
		}
		// Enqueue the message in priority order.
		else {
			ACE_DEBUG((LM_ERROR, ACE_TEXT("(%t) producer sending message %s\n"), buffer));
			// Allocate a new message, but have it "borrow" its memory from the buffer.
			ACE_Message_Block *mb;
			ACE_NEW_RETURN(mb, ACE_Message_Block(rb.size(), ACE_Message_Block::MB_DATA, 0, buffer), 0);
			// set the message length (no need for this since the consumer
			// does NOT read the length)
			// mb->wr_ptr(rb.size());
			// ACE_DEBUG ((LM_DEBUG, "enqueueing message of size %d\n", size));
			// Get message type into c variable
			char c=*buffer;
			switch (c) {
			case '1':
				// Enqueue in tail queue1
				if (pdata->msg_queue1->enqueue_tail(mb)==-1) {
					ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
				}
				break;

			case '2':
				// Enqueue in head queue1
				if (pdata->msg_queue1->enqueue_head(mb)==-1) {
					ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
				}
				break;

			case '3':
				// Enqueue in tail queue2
				if (pdata->msg_queue2->enqueue_tail(mb)==-1) {
					ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
				}
				break;

			case '4':
				// Enqueue in head queue2
				if (pdata->msg_queue2->enqueue_head(mb)==-1) {
					ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
				}
				break;

			default:
				ACE_DEBUG((LM_DEBUG, "(%t) <%c> DOING NOTHING!!!\n", c));
				break;
			}
		}
	}
	// finished ? then return
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) producer ending\n")));
	return NULL;
}

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	// Spawn off one thread that copies stdin to stdout in order of the size of each line.
	// Global thread manager.
	ACE_Thread_Manager thr_mgr;

	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) main starting\n")));
	// non portable code - watch out!!
	printf("my tid is %d\n", gettid());
	ACE_Message_Queue<ACE_MT_SYNCH> msg_queue1;
	ACE_Message_Queue<ACE_MT_SYNCH> msg_queue2;

	ProducerData pdata;
	pdata.msg_queue1=&msg_queue1;
	pdata.msg_queue2=&msg_queue2;
	if (thr_mgr.spawn(ACE_THR_FUNC(producer), &pdata, THR_NEW_LWP | THR_DETACHED)==-1) {
		ACE_ERROR_RETURN((LM_ERROR, "%t\n", "spawn producer"), 1);
	}
	ConsumerData c1data;
	c1data.msg_queue=&msg_queue1;
	c1data.id=1;
	if (thr_mgr.spawn(ACE_THR_FUNC(consumer), &c1data, THR_NEW_LWP | THR_DETACHED)==-1) {
		ACE_ERROR_RETURN((LM_ERROR, "%t\n", "spawn consumer1"), 1);
	}
	ConsumerData c2data;
	c2data.msg_queue=&msg_queue2;
	c2data.id=2;
	if (thr_mgr.spawn(ACE_THR_FUNC(consumer), &c2data, THR_NEW_LWP | THR_DETACHED)==-1) {
		ACE_ERROR_RETURN((LM_ERROR, "%t\n", "spawn consumer2"), 1);
	}
	// Wait for producer and consumers threads to exit.
	thr_mgr.wait();
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) main ending\n")));
	return EXIT_SUCCESS;
}
