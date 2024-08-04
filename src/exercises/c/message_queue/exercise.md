# Message queue exercise

* Implement a message queue using a simple 10 element array
* Use a pthread_mutex to protect the queue.
* You will need a condition variable to check if there are elements in the queue
* You will need the same condition variable to go to sleep in two conditions:
    * when you want to get a message from the queue are empty
    * when you want to put a message to the queue and the queue is full.
* Show that your queue works.

Hints:
* pthread_cond_*
