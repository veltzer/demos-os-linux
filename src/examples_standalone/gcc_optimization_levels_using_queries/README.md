# Explore different `gcc(1)` optimization levels

The makefile here helps you in querying gcc:
* seeing what optimizations in the compiler are enabled for each -O[N] number.
Note that -O4 and onwards are accepted by gcc but are useless as they provide the same optimizations as -O3...
* seeing what optimizations are with -Ofast, -O, -Os, -Og
* seeing what warnings does gcc support
