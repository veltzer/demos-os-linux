# Readers/Writer lock exercise

Implement a readers/writer lock WITHOUT using the pthread readers/writer
lock API. Use only pthread conditions (and pthread mutexes which are
a pre-requisite for using pthread conditions).
Bonus: make the lock wait for two writers before starting to evict readers.

You can see the pthread readers/writer lock API using `man -k pthread | grep rw`

Reminder: a readers/writer lock is a lock that allows either multiple
readers or just one writer to be inside the lock.

Only implement the following APIs:
`pthread_rwlock_init (3posix) - destroy and initialize a read-write lock object`
`pthread_rwlock_rdlock (3posix) - lock a read-write lock object for reading`
`pthread_rwlock_wrlock (3posix) - lock a read-write lock object for writing`
`pthread_rwlock_unlock (3posix) - unlock a read-write lock object`
`pthread_rwlock_destroy (3posix) - destroy and initialize a read-write lock object`

Demonstrate the use of your lock and show that it actually works.

TODO: the bonus is not yet solved.
