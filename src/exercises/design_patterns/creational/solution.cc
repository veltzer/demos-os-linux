#include <iostream>
#include <memory>
#include <string>

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

    void log(const std::string& message) {
        std::cout << "LOG: " << message << std::endl;
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
        std::cout << "Loading XML configuration" << std::endl;
    }
};

class JSONConfigManager : public ConfigurationManager {
public:
    void loadConfiguration() override {
        std::cout << "Loading JSON configuration" << std::endl;
    }
};

class ConfigFactory {
public:
    static std::unique_ptr<ConfigurationManager> createConfigManager(const std::string& type) {
        if (type == "XML") {
            return std::make_unique<XMLConfigManager>();
        } else if (type == "JSON") {
            return std::make_unique<JSONConfigManager>();
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

    return 0;
}
