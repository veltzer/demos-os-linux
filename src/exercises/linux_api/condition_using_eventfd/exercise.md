# Condition using `eventfd`

Implement the following pthread functions using eventfd(2):

```c
mypthread_cond_init (3) - operations on conditions
mypthread_cond_destroy (3) - operations on conditions
mypthread_cond_broadcast (3) - operations on conditions
mypthread_cond_signal (3) - operations on conditions
mypthread_cond_wait (3) - operations on conditions
```

with the same semantics as the pthread library API.

Bonus: suggest a way to improve the `pthread_cond` API to enable to signal a specific thread.
Implement your enhancement.
Demonstrate by using your newly defined condition API to implement a fine grained readers/writer lock.
