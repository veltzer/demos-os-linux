# Message Queue

* Implement a messaging network. Each `subscriber` will have an ID which will be equivalent to a queue ID. Each subscriber will read from the queue of his own ID and send messages to other subscribers queue ID.
* Error messages will be sent to an `Operator` on a designated queue like 166.
* Operator will be able to read from all queues and empty them.
* Try to identify resource full situations automatically and perform step 3 when they happened.
