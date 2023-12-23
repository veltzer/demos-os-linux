This is a real time design pattern exercise
===========================================
You are required to implement a real time design pattern
called "Exchanger" or, by another name, "Randezvous".
The idea is that two threads can meet, waiting for one another
in order to exchange a value. Each thread brings it's own
value to the encounter.

First thread to the encounter sleeps until the other thread arrives.
Second thread to the encounter wakes up the first one.
After the wakeup they exchange the values.

Write the exchanger as a template with one method exchange.

Accessible to both threads (global, or passed to them):
Exchanger<int> e;

Inside each thread:

	int value_for_me=e.exchange(value_for_him);

Demonstrate your use and prove (using assertions) that the
exchange has been made.

* You may use a condition and mutex in your solution (either pthread
or C++ based).
