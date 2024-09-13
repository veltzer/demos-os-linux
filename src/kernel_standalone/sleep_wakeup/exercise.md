# Sleep and Wakeup exercise

First Phase

Write a device driver (module) with only one operation:
ioctl(2).

This means that you will need to implement, instead of 'read' and 'write'
functions in the kernel, a 'ioctl'.

Show that you can call this ioctl from userspace.

For this phase, it really doesn't matter what the ioctl actually does.
For instance, you can just print a log message to the kernel log.

Of course, you will need to implement a user space application that calls
your 'ioctl' function in the kernel.

Documentation of how to implement an 'ioctl' operation in your module is in
the Bootlin slides at slides 224-226 (including).

A basic read or write module is in `https://github.com/veltzer/demos-linux/tree/master/src/kernel_standalone/mynull`

Second Phase
Create two ioctl code numbers:

```c
#define IOCTL_SLEEP 100
#define IOCTL_WAKEUP 101
```

Add code in your ioctl module that puts the calling process to sleep when IOCTL_SLEEP is called
and waked the processes sleeping when IOCTL_WAKEUP is called.
Demonstrate the use of your module with *two* user space processes. One going to sleep and one waking it up.
Use a wait queue to put processes to sleep and wake them up.

how to go to sleep and wakeup is in the slides in 307-313 (including).
