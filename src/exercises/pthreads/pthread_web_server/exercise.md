# Pthread Web Server

write a web server using the pthread API.
start by building a simple echo server
* see that the echo server works by using
    telnet localhost [your port number]
* change the server to be a stupid HTTP server.
    Read the ENTIRE response from a file.
    and send the file content as the response.
* see that a browser shows your HTML code.
* see that it also works for more than one client (open multiple browser tabs)
* create a client that works with multiple threads
    and issues requests to your server.
* use tcpdump to see the communication between
    your clients and server.

Notes:
to compile with pthread support use `g++ hello.cc -o hello -lpthread`
