A high performance echo server written in C on Linux
====================================================
This exercise requires you to build a multi-client echo
server in C under linux.

Design:
The design of the server is based on the epoll(7) subsystem.
So you would use just one thread to serve multiple clients.
The server you would build is an "echo" server, echoing back
whatever is echoed to it.
The server would still service *MULTIPLE* clients simultaneously but
*IT WOULD NOT* be multi-threaded.

Testing that it works:
You would test your server by connecting to it using multiple telnet
sessions.

Handling errors:
Remmember to handle errors from *ALL* system calls (you never know when
one of them breaks).

Bonus:
- have timeouts for threads.
	If a thread does not send anything for N seconds - disconnect
	from it.
- handle async write(2) correctly.
	Do not assume that you can echo back whatever the user send
	you without blocking, so use epoll for that as well.
- add a feature that enabled to bring down your server cleanly. Do
	that via a signalfd and add that to your epoll system.
