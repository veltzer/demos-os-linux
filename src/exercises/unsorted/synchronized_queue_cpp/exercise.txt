Use STL linked list + pthread cond + pthread mutex to create
a syncronzied message queue to be used to pass messages between
threads.

The two main methods: get, put

if you "get" when the queue is empty you go to sleep
"put" never goes to sleep and only adds to the queue.
(no limitation on the linkedlist size).

Have a nice Template OO wrapper for this.

	SyncQueue<MyMessageType>* q=new SyncQueue<MyMessageType>();
