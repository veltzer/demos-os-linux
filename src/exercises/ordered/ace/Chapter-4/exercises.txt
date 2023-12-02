Exercise 4
1. Write a Client and a Server programs that will communicate via Stream Socket.
2. You may assign the port internally or via argv[1] parameter.
3. The Server will read data message from STDIN and will send it to the Client via
	Stream Socket.
4. The Client will print the received data on the STDOUT.
5. When EOF is reached both programs will terminate gracefully.

Exercise 4a
1. Write a Client and a Server programs that will communicate via three Stream Socket.
2. You may assign the ports internally or via argv[1] parameter.
3. The ports may be with consecutive numbers.
4. The Server will read data message from STDIN and will send it to the Client via
	Stream Sockets.
5. Each message will include a message type.
6. Data will be sent according to the following rules:
	 a. Message type 1 and 2 1st socket
	 b. Message type 3 and 4 2nd socket
	 c. Any other type 3rd socket
7. The Client will have 3 threads each one will be listening to one of the three
	Stream Sockets.
8. Each message will be printed on the STDOUT.
9. When EOF is reached both programs will terminate gracefully.
