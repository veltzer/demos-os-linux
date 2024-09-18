# Exercise 9

* Write a program (it will be the parent process) that will create two children.
    Each one will be called a child process.
* The parent will accept two parameters which will be communication ports.
* First port will be used for communication with the first child. Second port will be used
    for communication with the second child.
* Create both children by using ACE `spawn_n` (NCHILDREN, options, pids) method.
* Use signal in order to set specific port for each child.
* Pass a single message from each process child to the parent then terminate the child.
* Terminate the parent after both children has completed their work.

## Exercise 9a

* Use Exercise 9 with the following modification:
* Wait for messages from both children using select.
