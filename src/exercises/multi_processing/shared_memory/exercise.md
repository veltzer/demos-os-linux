# Shared Memory

* Implement IRC network using shared memory. Every "subscriber" should have a piece of the shared memory assigned to him.
* Every shared memory piece assigned to a subscriber should have some space reserved to pointers (3 could be enough).
* Make sure that reader will not read information while modified. (not necessarily by using semaphores or locks)
