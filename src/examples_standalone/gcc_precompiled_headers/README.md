# Using precompiled headers with GCC

This example explores the features of gcc precompiled headers.

In order to see the difference in performance between using precompiled
headers and not do:

```shell
rm -f main*.precompiled.o
```

and then:

```shell
time make
```

and same for:

```shell
rm -f main*.noprecomp.o
```

and then:

```shell
time make
```

## TODO
* make a `make` target that shows the differences in speed.
