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
// #define ACE_NTRACE 0
#include <ace/OS_NS_stdio.h>
#include <ace/OS_NS_string.h>
#include <ace/config-lite.h>
#include <ace/OS_NS_unistd.h>
#include <ace/Read_Buffer.h>
#include <ace/Malloc_Base.h>
#include <ace/Message_Queue.h>
#include <ace/Activation_Queue.h>
#include <ace/Method_Request.h>
#include <ace/Task.h>
#include <ace/Future.h>
#include <ace/Auto_Ptr.h>

/*
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 * EXTRA_LINK_FLAGS=-lACE -ldl -lrt -lpthread
 */

static const long max_queue=LONG_MAX;

ACE_Message_Queue<ACE_MT_SYNCH> msg_queue(max_queue);
int ReadMessage(ACE_Message_Queue<ACE_MT_SYNCH> *msg_queue);

class MessageAgent {	// Proxy to the MessageAgent that is on the network.
public:
	MessageAgent() {
		ACE_TRACE(ACE_TEXT("MessageAgent::MessageAgentAgent"));
		status_result_=1;
	}

	int message_read(void) {
		ACE_TRACE(ACE_TEXT("MessageAgent::message_read"));
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Message Read ")));
		// Read message from queue
		ReadMessage(&msg_queue);
		ACE_OS::sleep(2);
		return(next_result_id());
	}

private:
	int next_result_id(void) {
		ACE_TRACE(ACE_TEXT("MessageAgent::next_cmd_id"));
		return(status_result_++);
	}

	int status_result_;
};

class MessageRequest : public ACE_Method_Request {
public:
	MessageRequest(MessageAgent & message, ACE_Future<int> &returnVal)
		: message_(message), returnVal_(returnVal) {
		ACE_TRACE(ACE_TEXT("MessageRequest::MessageRequest"));
	}

	virtual int call(void) {
		ACE_TRACE(ACE_TEXT("MessageRequest::call"));

		// message_read with the message.
		this->returnVal_.set(this->message_.message_read());
		return(0);
	}

private:
	MessageAgent& message_;
	ACE_Future<int> returnVal_;
};

class ExitMethod : public ACE_Method_Request {
public:
	virtual int call(void) {
		// Cause exit.
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Finally reached Future call()\n")));
		return(-1);
	}
};

class Scheduler : public ACE_Task_Base {
public:
	Scheduler() {
		ACE_TRACE(ACE_TEXT("Scheduler::Scheduler"));
		this->activate();
	}

	virtual int svc(void) {
		ACE_TRACE(ACE_TEXT("Scheduler::svc"));
		while(true) {
			// Dequeue the next method object
			std::unique_ptr<ACE_Method_Request> request(this->activation_queue_.dequeue());
			// Invoke the method request.
			if (request->call()==-1) {
				break;
			}
		}
		return(0);
	}

	int enqueue(ACE_Method_Request *request) {
		ACE_TRACE(ACE_TEXT("Scheduler::enqueue"));
		return(this->activation_queue_.enqueue(request));
	}

private:
	ACE_Activation_Queue activation_queue_;
};

class MessageAgentProxy {
	// This acts as a Proxy to the message impl object.

public:
	ACE_Future<int> status_update(void) {
		ACE_TRACE(ACE_TEXT("MessageAgentProxy::status_update"));
		ACE_Future<int> result;
		// Create and enqueue a method request on the scheduler.
		this->scheduler_.enqueue(new MessageRequest(this->message_, result));
		// Return Future to the client.
		return(result);
	}
	void exit(void) {
		ACE_TRACE(ACE_TEXT("MessageAgentProxy::exit"));
		this->scheduler_.enqueue(new ExitMethod);
	}

private:
	Scheduler scheduler_;
	MessageAgent message_;
};

class CompletionCallBack : public ACE_Future_Observer<int> {
public:
	CompletionCallBack(MessageAgentProxy& proxy) : proxy_(proxy) {
	}
	virtual void update(const ACE_Future<int>& future) {
		int result=0;
		((ACE_Future<int>)future).get(result);
		if(result==10) {
			this->proxy_.exit();
		}
	}

private:
	MessageAgentProxy& proxy_;
};

int GetMessageType(char* data) {
	// Read new line from stdin
	static ACE_Read_Buffer rb(ACE_STDIN);

	// read a single line from stdin
	// Allocate a new buffer.
	char* buffer=rb.read('\n');
	if(buffer==0) {
		// return message type zero when EOF is reached
		// Return 0 as message type
		return(0);
	} else {
		int type;
		sscanf(buffer, "%d", &type);
		// Remove the type from the buffer
		ACE_OS::sprintf(data, "%s", buffer + 2);
		return(type);
	}
}

int SendMessage(char* buffer, int type) {
	// ACE_DEBUG ((LM_DEBUG , ACE_TEXT ("SendMessage Line:%l\n")));
	ACE_Message_Block *mb;
	// get message size
	size_t size=ACE_OS::strlen(buffer);

	// Allocate a new message, but have it "borrow" its memory from the buffer.
	// ACE_NEW_RETURN (mb, ACE_Message_Block (size+1, ACE_Message_Block::MB_DATA, 0, buffer), 0);
	ACE_NEW_RETURN(mb, ACE_Message_Block(size + 1), 0);
	// Reserve location for buffer internally
	ACE_OS::strcpy(mb->wr_ptr(), buffer);
	// Then we DO NOT need to re-allocate buffer
	// Also we release ONLY the mb which holds the buffer
	mb->wr_ptr(size + 1);
	// Set Message Type
	mb->msg_type(type);
	switch(type) {
	case 1:
		// Enqueue in tail queue
		if (msg_queue.enqueue_tail(mb)==-1) {
			ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
		}
		break;
	case 2:
		// Enqueue in head queue
		if (msg_queue.enqueue_head(mb)==-1) {
			ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
		}
		break;
	case 3:
		// Enqueue in tail queue
		if (msg_queue.enqueue_tail(mb)==-1) {
			ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
		}
		break;
	case 4:
		// Enqueue in head queue
		if (msg_queue.enqueue_head(mb)==-1) {
			ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
		}
		break;
	default:
		// Enqueue the message at priority 10
		mb->msg_priority(10);
		if (msg_queue.enqueue_prio(mb)==-1) {
			ACE_ERROR((LM_ERROR, "(%t) %p\n", "put_next"));
		}
		break;
	}
	return(0);
}

int ReadMessage(ACE_Message_Queue<ACE_MT_SYNCH> *msg_queue) {
	ACE_Message_Block *mb;
	if (msg_queue->dequeue_head(mb)==-1) {
		return(1);
	}
	int length=ACE_Utils::truncate_cast<int> (mb->length());
	int type=mb->msg_type();
	if (length > 0) {
		ACE_DEBUG((LM_DEBUG, "(%d)-<%s>\n", type, mb->rd_ptr()));
	}
	// Free up the Message_Block. Buffer is includede and is NOT pointed.
	// Free the Memory Block
	mb->release();

	return(type);
}

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	MessageAgentProxy message;
	// Save place for up to 100 messages. If we are not sure we may use malloc
	// and have unlimited number.
	ACE_Future<int> results[100];
	char buffer[100];

	CompletionCallBack cb(message);

	int i=-1;
	// Set a value just initializing the while loop. It will terminate when EOF will generate type=0
	int type=1;
	while(type) {
		type=GetMessageType(buffer);
		if (!type) {
			break;
		}
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Sending message: <%s> (%d)\n"), buffer, type));
		// Send the Message
		SendMessage(buffer, type);
		// Increment index before being used, therefore we do not need
		// to decrease outside the loop if there is a need for that
		i++;
		results[i]=message.status_update();
		results[i].attach(&cb);
	}
	ACE_Thread_Manager::instance()->wait();
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) Finshed. After waiting for threads\n")));
	return(0);
}
