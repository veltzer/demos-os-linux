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
#include <stdio.h>	// for printf(3)
#include "time.hh"

void Time::setTime(int hours, int minutes, int seconds) {
	m_hours=hours;
	m_minutes=minutes;
	m_seconds=seconds;
}

void Time::Tick() {
	m_seconds++;
	if(m_seconds==60) {
		m_seconds=0;
		m_minutes++;
		if(m_minutes==60) {
			m_minutes=0;
			m_hours++;
			if(m_hours==24) {
				m_hours=0;
			}
		}
	}
}

void Time::printStd() {
	const char* part;
	int hours;
	if(m_hours>=12) {
		part="PM";
		hours=m_hours-12;
	} else {
		part="AM";
		hours=m_hours;
	}
	printf("%2d:%02d:%02d %s\n",
		hours,
		m_minutes,
		m_seconds,
		part
		);
}

void Time::printMilitary() {
	printf("%02d:%02d\n",
		m_hours,
		m_minutes
		);
}

// setters and getters...
void Time::setHours(int hours) {
	m_hours=hours;
}

int Time::getHours() {
	return m_hours;
}

void Time::setMinutes(int minutes) {
	m_minutes=minutes;
}

int Time::getMinutes() {
	return m_minutes;
}

void Time::setSeconds(int seconds) {
	m_seconds=seconds;
}

int Time::getSeconds() {
	return m_seconds;
}
