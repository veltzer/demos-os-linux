# Message Queue exercise

Write a multi-thread safe message queue to communicate
between threads.

The message queue should be an array of void* pointers.

Here is the API you should implement:

type:
    mq

API:
    int mq_init(mq*, const int maxsize);
        creates the queue with a maximum size of maxsize.
    int mq_destroy(mq*);
        destroys the queue (whatever is needed to be done).
    int mq_put(mq*, void*);
        put a message on the queue.
        goes to sleep if the queue is full.
    void* mq_get(mq*);
        get a message from the queue.
        goes to sleep if the queue is empty.

Demonstrate the use of this API.

make the mq type opaque so that changes to that type will not require recompilation
of your clients.

## Exercise 2: Create an queue abstraction for C programmers

`pthread_queue_t` is the type.

`void pthread_queue_init(pthread_queue_t*, int size);`
    will initialize the queue.
    the queue size is the maximum number of messages on the queue.
`void pthread_queue_put(pthread_queue_t*, void* msg);`
    will put a message on the queue.
    If the queue is full then this method sleeps until there is room
    in the queue.
`void* pthread_queue_get(pthread_queue_t*);`
    get a message from the queue.
    blocks if the queue is empty.
`void pthead_queue_destroy(pthread_queue_t*);`
    destroy all resources having to do with the queue.

use `pthread_condition` and `pthread_mutex` to solve.
