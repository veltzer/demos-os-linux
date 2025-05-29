# AAA exercise

Create a Linux kernel module that will expose one device file to `/dev`
named `/dev/aaa`.

The device will only support a read operation.

When someone reads from it, it will fill the users buffer with a's.

Like this:

```shell
$ cat /dev/aaa
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa....
```

Show that it works.

Allocate major and minor number either statically (hardcoded) or dynamically.

Copy as much at you want from [here](https://github.com/demos-os-linux/src/kernel{,_standalone})
