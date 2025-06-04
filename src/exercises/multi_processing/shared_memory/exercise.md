# Shared Memory

* Implement two processes "talking" through shared memory.
* The first process, let's call it the "server" comes up and defines a piece of shared memory (4K in size).
* The second process, let's call it the "client" comes up and attaches to the shared memory.
* The server changes the first integer in the 4K block incrementing it all every second.
* The client reads the first integer in the 4K block so that we could see it being incremented.

Notes:
* the API for sharing memory is: shmget, shmat, shmctl, ftok....
* in this exercise you need to write two processes: one server and one client.

Questions:
* Is the virtual address for the 4K block equal in the client and the server? Print it to find out.
