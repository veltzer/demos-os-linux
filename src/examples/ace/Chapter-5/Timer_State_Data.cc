/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <ace/OS_NS_time.h>
#include <ace/Log_Msg.h>
#include <ace/Reactor.h>
#include <ace/Event_Handler.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * Reactor examples
 * Timers & state data
 *
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 */

class TemperatureSensor {
public:
	TemperatureSensor(const char *location) : location_(location), count_(0), temperature_(0.0) {
	}
	const char *location() const {
		return(this->location_);
	}
	int querySensor(void) {
		return(++this->count_);
	}
	float temperature(void) const {
		return(this->temperature_);
	}

private:
	const char *location_;
	int count_;
	float temperature_;
};

class TemperatureQueryHandler : public ACE_Event_Handler {
public:
	TemperatureQueryHandler() : ACE_Event_Handler(), counter_(0), averageTemperature_(0.0) {
	}
	int handle_timeout(const ACE_Time_Value& current_time, const void* arg) {
		time_t epoch=((timespec_t)current_time).tv_sec;
		const TemperatureSensor *const_sensor=reinterpret_cast<const TemperatureSensor *>(arg);
		TemperatureSensor *sensor=const_cast<TemperatureSensor *>(const_sensor);
		int queryCount=sensor->querySensor();
		this->updateAverageTemperature(sensor);
		ACE_DEBUG((LM_INFO, ACE_TEXT("%s\t%d/%d\t%.2f/%.2f\t%s\n"), sensor->location(), ++this->counter_, queryCount, this->averageTemperature_, sensor->temperature(), ACE_OS::ctime(&epoch)));
		return(0);
	}

private:
	void updateAverageTemperature(TemperatureSensor *) {
	}
	int counter_;
	float averageTemperature_;
};

// Create a SIGINT handler so that we can exit
// the program politely
class SigintHandler : public ACE_Event_Handler {
public:
	int handle_signal(int signum, siginfo_t* =0, ucontext_t* =0) {
		if(signum==SIGINT) {
			ACE_Reactor::instance()->end_reactor_event_loop();
		}
		return(0);
	}
};

int ACE_TMAIN(int argc, ACE_TCHAR** argv, ACE_TCHAR** envp) {
	TemperatureQueryHandler *temperatureMonitor=new TemperatureQueryHandler();
	ACE_Time_Value initialDelay(3);
	ACE_Time_Value intervalOne(5);
	TemperatureSensor *sensorOne=new TemperatureSensor("Kitchen");
	ACE_Reactor::instance()->schedule_timer(temperatureMonitor, sensorOne, initialDelay, intervalOne);
	ACE_Time_Value intervalTwo(4);
	TemperatureSensor *sensorTwo=new TemperatureSensor("Foyer");
	ACE_Reactor::instance()->schedule_timer(temperatureMonitor, sensorTwo, initialDelay, intervalTwo);
	SigintHandler *handleExit=new SigintHandler();
	ACE_Reactor::instance()->register_handler(SIGINT, handleExit);
	ACE_Reactor::instance()->run_reactor_event_loop();
	return EXIT_SUCCESS;
}
