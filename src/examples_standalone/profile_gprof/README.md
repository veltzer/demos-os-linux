# Profiling your code using `gprof(1)`

This example shows how to profile using `gprof(1)`.
What you should take notice of:
* the makefile: see how to compile with `gprof(1)` support.
    (`-pg`, allowed with `-OXXX`)
* the code. If you exit with `_exit(2)` then you won't get profile
    info. If you exit with exit or normally then you do
    get profile info. take heed!
