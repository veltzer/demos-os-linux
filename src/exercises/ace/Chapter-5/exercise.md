# Exercise 5

* Write a Reactor program that will listen to two or more consecutive ports.
    (If your software works for 2 ports, 3 or more is easy).
* You may assign the port internally (for example starting with ACE_DEFAULT_SERVER_PORT = 20002)
    or using argv[1] as starting port.
* The Reactor will print each message received from the appropriate port. (Use some identification
    in the output in order to differ among the ports)
* You may test the program using the telnet as follows:
    telnet localhost port
    Then any input from the telnet utility is received by the reactor.
* Finally, use the input data in order to terminate the connection. Then, the Reactor will close
    the opened ports and will terminate gracefully.
    Note: Since telnet does not close properly (only the reactor does the closing), you may face
    a problem using the same port again before appropriate timeout.

## Exercise 5a
* Write a Connect program that will send data to two of the consecutive ports.
* The program will read messages each one will have a message type - an integer.
* Messages having type 1 or 2 will be sent to the first port.
* Messages having any other type (other than 1 or 2) will be sent to the second port.
* Finally, use the input data in order to terminate the connection. Then, the connector will close
    the opened ports and will terminate gracefully. This will also force the Reactor to close
    gracefully as was done in the previous exercise.

## Exercise 5b
* Use the program 5 (not 5a) and add signal -1 that will close the program gracefully.
    Hint: Print pid and use 'kill -1 pid' command. Also, use ACE_OS::sleep() to slow the input. This
    will help you while trying to enter the command using another terminal.
