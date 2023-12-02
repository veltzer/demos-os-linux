Message Queue exercise
1. Implement a "Mirs" network. Each "subscriber" will have an ID which will be equivalent to a queue ID. Each subscriber will read from the queue of his own ID and send messages to other subscribers queue ID.
2. Error messages will be sent to an “Operator” on a designated queue like 166.
3. Operator will be able to read from all queues and empty them.
4. Try to identify resource full situations automatically and perform step 3 when they happened.
