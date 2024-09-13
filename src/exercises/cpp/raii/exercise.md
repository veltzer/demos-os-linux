# Exercise: Implementing RAII for File Handling in C++

## Objective

Implement a C++ class that demonstrates the Resource Acquisition Is Initialization (RAII) idiom for managing file resources.

## Requirements

* Create a class named `File` that manages a file handle (FILE*).
* The class should:
    * Open a file in its constructor.
    * Close the file in its destructor.
    * Provide a method to write to the file.
    * Handle potential errors (e.g., file not found) by throwing exceptions.
    * Ensure that the file is properly closed even if an exception is thrown.
    * Prevent copying of `File` objects to avoid double deletion of resources.
* Implement a function `useFile` that demonstrates the usage of your `File` class.
* In the `main` function, call `useFile` to show how RAII manages the file resource.

## Bonus

* Add additional file operations (e.g., read, seek) to the `File` class.
* Implement move semantics for the `File` class.

## Expected Outcome

Your program should demonstrate:

* Automatic resource management (file opens when `File` object is created, closes when it's destroyed).
* Exception safety (file closes even if an error occurs during writing).
* Prevention
