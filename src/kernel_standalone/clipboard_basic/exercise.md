Write a kernel module that holds a buffer in kernel space.
The kernel module implements a character device driver and thus
allows user space to read and write to that buffer.

* Do not worry about synchronization
* Do not worry about going to sleep while waiting for the buffer to free up so that
	you can write.
* Do not worry about going to sleep while waiting for the buffer to get filled up so
	that you can read.
* Do not worry about permissions when accessing your device. Do it as root. I know this
is not the right way - just don't worry about it for now.

test your driver this way:
$ echo "hello" > /dev/mybuffer
or
$ cat /etc/passwd > /dev/mybuffer
and then:
$ cat /dev/mybuffer
