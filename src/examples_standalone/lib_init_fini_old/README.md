# Library initialization and cleanup old API

This demonstration show that:
* You can write you own init and finish function to be executed when an application starts
    using your dll and when it finishes so.
* These hooks will only be called once per application and will NOT be called for threads
    and forks.
* You must register them using the right flags to the linker.
    `-Xlinker -init -Xlinker my_init -Xlinker -fini -Xlinker my_fini`
* You can also set a build id.
    `-Xlinker --build-id=0xaabbcc`
* Notice that the order of initialization is derived from the order of linkage.
    Change the order of linkage and you get different initialization order.
