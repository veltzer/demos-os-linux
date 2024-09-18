# Exercise 2

* Write a program that will use the example Exercise2.cpp.sample
* Use the producer to read from a file messages from a file until EOF.
* Use 4 types of messages and do the following for each type:
    * Send the message to Queue1.
    * Send the message to Queue1 but to the head of the queue.
    * Send the message to Queue2.
    * Send the message to Queue2 but to the head of the queue.
    Use message block in order to feed the queues.
* Write consumer1 that will read messages from Queue1 and will print them to the STDOUT.
* Write consumer2 that will read messages from Queue2 and will print them to the STDOUT.
* When EOF is reached specify end of data for the two consumers and terminate the program.

Does the message type change anything in message order?
If not? what can you do?

## Exercise 2-A
Modify Exercise 2 accordingly:
* Set message type 1-4 using: `mb->msg_type(int);`
* For any other type create message type 5, send it to Queue1 setting High priority of 10.
* Such message when received will be modified to type 4 and re-sent (by the appropriate
    consumer) to Queue2.

## Exercise 2-B
Modify Exercise 2 accordingly:
* Set message type 1-4 using: `mb->msg_type (int);`
* Each message line will contain 3 fields: type delay in sec. and the message body.
* Each time a message is read by the producer it will activate a timer according to the
    specified delay in the message. Please note that each delay is from the previous message.
    The delay is a floating point value in seconds. It may be even a fraction of a second.
    remember that producer will send messages to Queue1 and Queue2.
* The timer will activate the consumer and the consumer will process a single message from
    the appropriate queue. (Remember that there are Queue1 and Queue2).
* Do not neglect the termination condition.
* Use appropriate built in function to print the message body without the type and the delay
    that appear in the data stream.
