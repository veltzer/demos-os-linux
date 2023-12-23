# Simple Watchdog

Write a watchdog type double process.

The watchdog is the parent
The interesting process is the child

The parent will restart the child whenever the child
dies of *abnormal circumstances*.

If the child dies of normal circumstances, the watchdog will
terminate as well.

Use the following system calls
* `fork(2)`
* `wait(2)`
