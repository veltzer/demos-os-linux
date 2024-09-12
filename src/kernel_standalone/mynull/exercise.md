# Write a `/dev/null` like device driver.

Use pieces of code from:
[link](https://github.com/veltzer/demos-linux/tree/master/src/kernel_standalone/mynull)

Notes
* Call your `/dev` file `mynull` in order not to collide with the actuall `/dev/null`
* Try not to crash your kernel.
* Show that your driver works.
* Don't forget to move the offset with every write operation
* Don't forget to write a message to the kernel log on every write operation.
