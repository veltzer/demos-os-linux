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
#include <ace/config-lite.h>
#include <ace/OS_NS_unistd.h>
#include <ace/Activation_Queue.h>
#include <ace/Method_Request.h>
#include <ace/Task.h>
#include <ace/Future.h>
#include <ace/Auto_Ptr.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 */

// Proxy to the HA_Controller that is on the network.
class HA_ControllerAgent {
public:
	HA_ControllerAgent() {
		ACE_TRACE(ACE_TEXT("HA_ControllerAgent::HA_ControllerAgent"));
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("Entered HA_ControllerAgent\n")));
		status_result_=0;
	}
	int status_update(void) {
		ACE_TRACE(ACE_TEXT("HA_ControllerAgent::status_update"));
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("Obtaining a status_update in %t ") ACE_TEXT("thread of control\n")));
		// Simulate time to send message and get status.
		ACE_OS::sleep(2);
		return(next_result_id());
	}

private:
	int next_result_id(void) {
		ACE_TRACE(ACE_TEXT("HA_ControllerAgent::next_cmd_id"));
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("Entered next_result_id\n")));
		status_result_++;
		return(status_result_);
	}
	int status_result_;
};

class StatusUpdate : public ACE_Method_Request {
public:
	StatusUpdate(HA_ControllerAgent & controller, ACE_Future<int> &returnVal) : controller_(controller), returnVal_(returnVal) {
		ACE_TRACE(ACE_TEXT("StatusUpdate::StatusUpdate"));
	}
	virtual int call(void) {
		ACE_TRACE(ACE_TEXT("StatusUpdate::call"));
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("Entered call\n")));
		// status_update with the controller.
		this->returnVal_.set(this->controller_.status_update());
		return(0);
	}

private:
	HA_ControllerAgent& controller_;
	ACE_Future<int> returnVal_;
};

class ExitMethod : public ACE_Method_Request {
public:
	virtual int call(void) {
		// Cause exit.
		return(-1);
	}
};

class Scheduler : public ACE_Task_Base {
public:
	Scheduler() {
		ACE_TRACE(ACE_TEXT("Scheduler::Scheduler"));
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("Entered Scheduler Before this->activate()\n")));
		this->activate();
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("Leaving Scheduler After this->activate()\n")));
	}
	virtual int svc(void) {
		ACE_TRACE(ACE_TEXT("Scheduler::svc"));
		while(true) {
			// Dequeue the next method object
			ACE_DEBUG((LM_DEBUG, ACE_TEXT("In Scheduler::svc waiting for queue data dequeue\n")));
			std::unique_ptr<ACE_Method_Request> request(this->activation_queue_.dequeue());
			// Invoke the method request.
			if(request->call()==-1) {
				break;
			}
		}
		return(0);
	}
	int enqueue(ACE_Method_Request *request) {
		ACE_TRACE(ACE_TEXT("Scheduler::enqueue"));
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("In Scheduler::enqueue\n")));
		return(this->activation_queue_.enqueue(request));
	}

private:
	ACE_Activation_Queue activation_queue_;
};

class HA_ControllerAgentProxy {
	// This acts as a Proxy to the controller impl object.

public:
	ACE_Future<int> status_update(void) {
		ACE_TRACE(ACE_TEXT("HA_ControllerAgentProxy::status_update"));
		ACE_Future<int> result;
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("In HA_ControllerAgentProxy::status_update\n")));

		// Create and enqueue a method request on the scheduler.
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("Activating enque\n")));
		this->scheduler_.enqueue(new StatusUpdate(this->controller_, result));

		// Return Future to the client.
		return(result);
	}
	void exit(void) {
		// FUZZ: enable check_for_lack_ACE_OS
		ACE_TRACE(ACE_TEXT("HA_ControllerAgentProxy::exit"));
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("In HA_ControllerAgentProxy::exit\n")));
		this->scheduler_.enqueue(new ExitMethod);
	}

private:
	Scheduler scheduler_;
	HA_ControllerAgent controller_;
};

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Constructing HA_ControllerAgentProxy controller\n")));
	HA_ControllerAgentProxy controller;
	const int numResults=10;
	ACE_Future<int> results[numResults];
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Activating the controller.status_update\n")));
	for(int i=0; i<numResults; i++) {
		results[i]=controller.status_update();
	}
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("We simulate doing other work by sleep(3)\n")));
	// Do other work.
	ACE_OS::sleep(5);
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("------Until now we did not request any data----\n")));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("After the sleep we are going to get the results\n")));
	// Get results...
	for(int j=0; j<numResults; j++) {
		int result=0;
		results[j].get(result);
		ACE_DEBUG((LM_DEBUG, ACE_TEXT("New status_update %d\n"), result));
	}
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Calling the controller exit \n")));
	// Cause the status_updater threads to exit.
	controller.exit();
	ACE_Thread_Manager::instance()->wait();
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("Done\n")));
	return EXIT_SUCCESS;
}
