# Exercise 4

* Write a Client and a Server programs that will communicate via Stream Socket.
* You may assign the port internally or via argv[1] parameter.
* The Server will read data message from STDIN and will send it to the Client via
    Stream Socket.
* The Client will print the received data on the STDOUT.
* When EOF is reached both programs will terminate gracefully.

## Exercise 4a
* Write a Client and a Server programs that will communicate via three Stream Socket.
* You may assign the ports internally or via argv[1] parameter.
* The ports may be with consecutive numbers.
* The Server will read data message from STDIN and will send it to the Client via
    Stream Sockets.
* Each message will include a message type.
* Data will be sent according to the following rules:
    * Message type 1 and 2 first socket
    * Message type 3 and 4 second socket
    * Any other type 3rd socket
* The Client will have 3 threads each one will be listening to one of the three
    Stream Sockets.
* Each message will be printed on the STDOUT.
* When EOF is reached both programs will terminate gracefully.
