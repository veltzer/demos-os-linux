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
#include <ace/config-lite.h>
#include <ace/OS_NS_unistd.h>
#include <ace/Activation_Queue.h>
#include <ace/Method_Request.h>
#include <ace/Task.h>
#include <ace/Future.h>
#include <cstdlib>

using namespace std;

/*
 * EXTRA_COMPILE_CMD=pkg-config --cflags ACE
 * EXTRA_LINK_CMD=pkg-config --libs ACE
 */

class HA_ControllerAgent{
	// Proxy to the HA_Controller that is on the network.

public:
	HA_ControllerAgent() {
		ACE_TRACE("HA_ControllerAgent::HA_ControllerAgent");
		status_result_=1;
	}

	int status_update(void) {
		ACE_TRACE("HA_ControllerAgent::status_update");
		ACE_DEBUG((LM_DEBUG, "Obtaining a status_update in %t thread of control\n"));
		// Simulate sending message to controller
		// and getting status.
		ACE_OS::sleep(2);
		return next_result_id();
	}

private:
	int next_result_id(void) {
		ACE_TRACE("HA_ControllerAgent::next_cmd_id");
		return status_result_++;
	}

	int status_result_;
};

class StatusUpdate: public ACE_Method_Request {
public:
	StatusUpdate(HA_ControllerAgent & controller, ACE_Future<int> &returnVal) : controller_(controller), returnVal_(returnVal) {
		ACE_TRACE("StatusUpdate::StatusUpdate");
	}

	virtual int call(void) {
		ACE_TRACE("StatusUpdate::call");

		// status_update with the controller.
		this->returnVal_.set(this->controller_.status_update());
		return 0;
	}

private:
	HA_ControllerAgent& controller_;
	ACE_Future<int> returnVal_;
};

class ExitMethod: public ACE_Method_Request {
public:
	virtual int call(void) {
		// Cause exit.
		return -1;
	}
};

class Scheduler: public ACE_Task_Base {
public:
	Scheduler() {
		ACE_TRACE("Scheduler::Scheduler");
		this->activate();
	}

	virtual int svc(void) {
		ACE_TRACE("Scheduler::svc");
		while(true) {
			// Dequeue the next method object
			unique_ptr<ACE_Method_Request> request(this->activation_queue_.dequeue());
			// Invoke the method request.
			if(request->call()==-1) {
				break;
			}
		}
		return 0;
	}

	int enqueue(ACE_Method_Request *request) {
		ACE_TRACE("Scheduler::enqueue");
		return this->activation_queue_.enqueue(request);
	}

private:
	ACE_Activation_Queue activation_queue_;
};

class HA_ControllerAgentProxy{	// This acts as a Proxy to the controller impl object.
public:
	ACE_Future<int> status_update(void) {
		ACE_TRACE("HA_ControllerAgentProxy::status_update");
		ACE_Future<int> result;

		// Create and enqueue a method request on the scheduler.
		this->scheduler_.enqueue(new StatusUpdate(this->controller_, result));

		// Return Future to the client.
		return result;
	}

	// FUZZ: disable check_for_lack_ACE_OS
	void exit(void) {
		// FUZZ: enable check_for_lack_ACE_OS
		ACE_TRACE("HA_ControllerAgentProxy::exit");
		this->scheduler_.enqueue(new ExitMethod);
	}

private:
	Scheduler scheduler_;
	HA_ControllerAgent controller_;
};

class CompletionCallBack: public ACE_Future_Observer<int> {
public:
	CompletionCallBack(HA_ControllerAgentProxy & proxy) : proxy_(proxy) {
	}
	virtual void update(const ACE_Future<int>& future) {
		int result=0;
		((ACE_Future<int>)future).get(result);
		ACE_DEBUG((LM_INFO, "(%t) New Status %d\n", result));
		if(result==10) {
			this->proxy_.exit();
		}
	}

private:
	HA_ControllerAgentProxy& proxy_;
};

int main() {
	HA_ControllerAgentProxy controller;

	ACE_Future<int> results[10];
	CompletionCallBack cb(controller);
	for(int i=0; i < 10; i++) {
		results[i]=controller.status_update();
		results[i].attach(&cb);
	}
	ACE_Thread_Manager::instance()->wait();
	return EXIT_SUCCESS;
}
