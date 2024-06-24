# C++ Creational Design Patterns Exercise

## Objective
Implement a simple logging system using the Singleton pattern and a configuration manager using the Factory Method pattern.

## Part 1: Singleton Logger

Create a `Logger` class that follows the Singleton pattern:

1. The `Logger` should have a private constructor.
2. Implement a static method `getInstance()` that returns the single instance of the `Logger`.
3. Add a method `log(string message)` that prints the message to the console.

## Part 2: Configuration Factory

Create a `ConfigurationManager` class hierarchy using the Factory Method pattern:

1. Define an abstract base class `ConfigurationManager` with a pure virtual method `loadConfiguration()`.
2. Implement two concrete classes: `XMLConfigManager` and `JSONConfigManager`.
3. Each concrete class should implement `loadConfiguration()` to print a message indicating the type of configuration being loaded.
4. Create a `ConfigFactory` class with a static method `createConfigManager(string type)` that returns the appropriate configuration manager based on the input.

## Example Usage

```cpp
int main() {
    // Singleton Logger
    Logger::getInstance().log("Application started");

    // Configuration Factory
    ConfigFactory factory;
    unique_ptr<ConfigurationManager> config = factory.createConfigManager("XML");
    config->loadConfiguration();

    Logger::getInstance().log("Configuration loaded");

    return 0;
}
```

## Expected Output

```
Application started
Loading XML configuration
Configuration loaded
```

## Bonus Challenge

Extend the ConfigurationManager to use the Builder pattern for creating complex configuration objects.
