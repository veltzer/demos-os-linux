# Volatile, Atomic and more

You have a several threads (2 are enough but more if you want) that do some work for you.
Every time a thread finishes a piece of work it want to increase a counter. The counter is global.

You want to make the global counter that starts at 0, represents all the pieces of work done by all threads.

You can use volatile and atomics.

See what happens when you drop `volatile` and 

Fix the problem and compare all the solution you can find to this problem in terms of efficiency.

Hints:
* make sure you compile with optimization (-O2).
* threads can be created either using the `pthread` api or C++ thread support.
