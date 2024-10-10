# `LD_PRELOAD`

If you want your symbols to be called instead of some shared libraries symbols,
lets say that for some reason you don't want to change the original library:
* you don't have the source code.
* the library is standard - you don't want to touch it.
* the library is supplied by a third party - you don't want to touch it.
* the library may have different versions on different platforms - you don't want to touch it.
* you have the source code but it's too complicated and big.
* many other reasons I can't think about now...

There are 3 ways to achieve this:
* create a library of your own with the same interface and link it instead of the original or in addition to the original but before it. Your symbols will take precedence. In this case you interfere with the linking procedure of the application. You run the application as usual. In this case you cannot call the original libraries version.
* create a library of your own with the same interface and load it dynamically before calling the executable using the LD_PRELOAD environment variable. In this case you do not need to interfere with the linking of procedure of the application but rather with the running of the application (or rather the environment of the application). In this case you can call the original libraries version by doing dlsym(NULL,name) and calling the original from your code. You can also set default preloaded libraries for the entire system in /etc/ld.so.preload.
* Another way to call the original libraries version then create a library of your own with the same interface, load the original library using dlopen and call it whenever you want.

## References
[here](http://www.linuxjournal.com/article/7795)
