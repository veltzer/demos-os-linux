# `ld(1)` and link order

This example investigates link order in the GNU toolchain.

This is the dependency graph for this example:

main.elf -> liba.so -> libb.so

What are the results?
* if `liba.so` supplies function `a` and `libb.so` supplies function `b` and `a` needs `b`
    and you like in the right order (`-la -lb`) then all is OK. even if liba.so does
    not carry the information that it is dependent on `libb.so`.
* if, in the same case as above, you link in the wrong order (`-lb -la`) you will get
    link errors. Link order matters.
* to make order not matter you can use grouping.
    `-Wl,--start-group -lb -la -Wl,--end-group`
    but this is not advised as you should know the order of the linking.
* if you link `liba.so` with `libb.so` (`gcc -shared -o liba.so -L. -lb`) then
    you `liba.so` will carry this information and you can see it using
    `ldd liba.so`
* if `liba.so` was linked with `libb.so` then when linking with `liba.so` there
    is no need to specify `-lb`. But you do need the compiler to know
    where 'libb.so' resides. This cannot be done using the `-L` flag.
    It can be done using the `LD_LIBRARY_PATH` variable
    or via the `-rpath-link` flag to the linker

References:
[1](http://stackoverflow.com/questions/45135/linker-order-gcc)
