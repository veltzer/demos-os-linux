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
#include <ace/Stream.h>
#include <ace/Task.h>
#include <ace/Message_Block.h>
#include <unistd.h>	// for sleep(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <err_utils.h>	// for CHECK_ZERO()

/*
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 * EXTRA_LINK_FLAGS_AFTER=-lACE -ldl -lrt -lpthread
 */

typedef ACE_Stream<ACE_MT_SYNCH> MT_Stream;
typedef ACE_Module<ACE_MT_SYNCH> MT_Module;
typedef ACE_Task<ACE_MT_SYNCH> MT_Task;

static int ProducerData=0;
class Consumer : public MT_Task {
public:
	// Initialize Consumer.
	virtual int open(void *) {
		// <activate> is inherited from class Task.
		return(activate(THR_BOUND));
	}

	// Enqueue the message on the Message_Queue
	// for subsequent processing in <svc>.
	virtual int put(ACE_Message_Block *mb, ACE_Time_Value *tv=0) {
		// <putq> is inherited from class Task.
		return(putq(mb, tv));
	}

	// Receive message from producer and print to stdout.
	virtual int svc(void);
};

// Define the Producer interface
class Producer : public MT_Task {
public:
	// Initialize Producer.
	virtual int open(void *) {
		// activate() is inherited from class Task.
		return(activate(THR_BOUND));
	}

	// Read data from stdin and pass to consumer.
	virtual int svc(void);
};

int Producer::svc(void) {
	if(ProducerData) {
		while(true) {
			printf("Second Program Running\n");
			CHECK_ZERO(sleep(1));
		}
	}
	while(true) {
		ACE_Message_Block *mb;
		// Allocate a new message.
		ACE_NEW_RETURN(mb, ACE_Message_Block(BUFSIZ), -1);
		// Keep reading stdin, until we reach EOF.
		ssize_t n=ACE_OS::read(ACE_STDIN, mb->wr_ptr(), mb->size());
		if(n<=0) {
			// Send shutdown message to other thread and exit.
			mb->length(0);
			put_next(mb);
			break;
		} else {
			// Adjust write pointer.
			mb->wr_ptr(n);
			// Send the message to the other thread.
			this->put_next(mb);
		}
	}
	return(0);
}

int Consumer::svc(void) {
	ACE_Message_Block *mb=0;
	// Keep looping, reading a message from the queue,
	// until we get a 0 length message, then quit.
	while(true) {
		int result=getq(mb);
		if(result==-1) {
			break;
		}
		int length=mb->length();
		if(length>0) {
			ACE_OS::write(ACE_STDOUT, mb->rd_ptr(), length);
		}
		mb->release();
		if(length==0) {
			break;
		}
	}
	return(0);
}

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	// Control hierarchically-related active objects.
	MT_Stream stream;
	// All processing is performed in the
	// Stream after <push>’s complete.
	stream.push(new MT_Module("Consumer", new Consumer));
	stream.push(new MT_Module("Producer", new Producer));
	ProducerData=1;
	stream.push(new MT_Module("Producer1", new Producer));
	// Barrier synchronization: wait for
	// the threads, to exit, then exit the main thread.
	ACE_Thread_Manager::instance()->wait();
	return EXIT_SUCCESS;
}
