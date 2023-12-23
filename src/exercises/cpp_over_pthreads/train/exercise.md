# Train

A train exercise using `pthread_barrier(3)`

Simulate a number of threads (say 4) that work on a cyclic buffer.
Synchronize them using a `pthread_barrier(3)`
Thread 0 will "create data" (as if coming from hardware).
Thread 1 will work on the data that thread 0 created and so on.
And so on.
Thread 3 will simulate sending the data out of the system.

Add randomized sleep to all threads to simulate a real live situation.
