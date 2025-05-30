/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <iostream>
#include <memory>
#include <string>
#include <cstdlib>

using namespace std;

// Part 1: Singleton Logger
class Logger {
private:
	Logger() {}
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;

public:
	static Logger& getInstance() {
		static Logger instance;
		return instance;
	}

	void log(const string& message) {
		cout << "LOG: " << message << endl;
	}
};

// Part 2: Configuration Factory
class ConfigurationManager {
public:
	virtual void loadConfiguration() = 0;
	virtual ~ConfigurationManager() = default;
};

class XMLConfigManager : public ConfigurationManager {
public:
	void loadConfiguration() override {
		cout << "Loading XML configuration" << endl;
	}
};

class JSONConfigManager : public ConfigurationManager {
public:
	void loadConfiguration() override {
		cout << "Loading JSON configuration" << endl;
	}
};

class ConfigFactory {
public:
	static unique_ptr<ConfigurationManager> createConfigManager(const string& type) {
		if (type == "XML") {
			return make_unique<XMLConfigManager>();
		} else if (type == "JSON") {
			return make_unique<JSONConfigManager>();
		}
		return nullptr;
	}
};

// Main function to demonstrate usage
int main() {
	// Singleton Logger
	Logger::getInstance().log("Application started");
	// Configuration Factory
	auto config = ConfigFactory::createConfigManager("XML");
	if (config) {
		config->loadConfiguration();
	}

	Logger::getInstance().log("Configuration loaded");
	return EXIT_SUCCESS;
}
