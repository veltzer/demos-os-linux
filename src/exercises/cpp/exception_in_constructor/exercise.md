# Throwing Exceptions from Constructors

Write a class called `Resource` that wraps the acquisition and release of a system resource (e.g. file handle, network socket). It should implement:

* A constructor that acquires the resource
* A destructor that releases the resource
* Member functions to use the resource

## Requirements

* The constructor should validate any arguments passed to it. If invalid, it should throw an exception before acquiring the resource.

* The constructor should attempt to acquire the resource. If unsuccessful, it should throw an exception describing the failure that includes the system error code.

* The class should implement RAII by acquiring in its constructor and releasing in its destructor.

* If exceptions are possible when using the resource, carefully consider if those functions need additional try/catch blocks.

* Demonstrate using the class by wrapping `fopen` and `fclose` calls for working with a file.

## Things to Demonstrate

* Propagation of the constructor exception out to calling code.
* Catch constructor exceptions and handle failures locally.
* Design function signatures and interaction with resource exceptions.
