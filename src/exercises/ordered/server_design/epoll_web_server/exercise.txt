A high performance HTTP server written in C++ on Linux
======================================================
This exercise requires you to build a multi-client capable HTTP server
in C++ under linux.

Design:
The design of the server is based on the epoll(2) subsystem.
So you would use just one thread to serve multiple clients.
Your server would receive a service directory. Whenever a request for
a file from the service directory comes along then it would just serve
the content of that file.
The server would still service *MULTIPLE* clients simultaneously but
*IT WOULD* not be multi-threaded.

Writing the code:
Write your code in multiple files as a good design would require.
Write a Makefile to build your project (it's not more than a couple
of lines long).

Testing that it works:
You would test your server by connecting to it using multiple web browsers
or multiple tabs from a single web browser.

Handling errors:
Try to abstract error handling and the operating system low level C api
into C++ classes as much as you can (think of ACE Reactor and Proactor
patterns when doing this).
Remmember to handle errors from *ALL* system calls (you never know when
one of them breaks).

The HTTP protocol
=================
You need just a little bit understanding of the HTTP protocol in order to:
1) parse the incoming request and get at the URL which is required.
2) output a valid HTTP response with the content attached at the end.
You can find the relevant details in the following link.
HTTP protocol made easy:
http://www.jmarshall.com/easy/http/#sample

Phase 2:
========
Add multi threading to your server. The extra threads will create dynamic
content to be delivered to the client when the requested URL is not the
name of a physical file. Use the pthread(3) API to create and manage the
extra threads. Use the rendevous design pattern to arrange for the threads
to meet and exchange data - the servers main thread will give the worker
thread a new request for data while the worker thread will return the
results for the last request. One worker thread is enough for this exercise.
