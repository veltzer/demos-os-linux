# An advanced example of `dlopen(3)`

This example shows how to dynamically call a function from a dll which is not known at compile time.

This example uses the `dlopen(3)` set of functions.

These set of functions can be used for various purposes:
* Provide a plugin functionality to an application (think of a media player written in C/C++ that can be extended with many coded plugins).
* Enable an application to call functions of a library whose dependencies conflict with the dependencies of the application (different C library etc).

Usage example:

```shell
./run_func_from_dll /usr/lib/libm.so sin 6.28
```

Notes:
You need to know the signature of the functions you are calling (this is quite a fair requirement). This example assumes that you are calling a function that receives a single double argument and returns a double.
