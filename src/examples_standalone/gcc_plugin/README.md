# Building plugins for `gcc(1)`

## Notes
It turns out the plugin must be built with g++ since the support library for the
plugin has all the symbols mangled.

References
[1](https://gcc.gnu.org/onlinedocs/gccint/Plugins-building.html#Plugins-building)
[2](http://lwn.net/Articles/457543/)
[3](http://lwn.net/Articles/458806/)
[4](http://stackoverflow.com/questions/16957130/problems-with-building-the-gcc-plugin)
