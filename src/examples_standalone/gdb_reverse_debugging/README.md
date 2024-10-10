# Reverse debugging in `gdb(1)`

In order to do reverse debugging:
* compile the program with debug information.
* run `gdb`:

```shell
shell$ gdb main.elf
```

* set a break point where you want recording to start say:

```gdb
(gdb) break main
```

* now start recording:

```gdb
(gdb) target record-full
```

* set another breakpoint where the bug happens:

```gdb
(gdb) break main.cc:28
```

* now get to that break point:

```gdb
(gdb) continue
or just
(gdb) c
```

* now back up using reverse-* commands:

```gdb
(gdb) reverse-next
or just:
(gdb) rn
```

References:
[1](http://www.gnu.org/software/gdb/news/reversible.html)
[2](http://stackoverflow.com/questions/1470434/how-does-reverse-debugging-work)
[3](https://www.sourceware.org/gdb/wiki/ProcessRecord/Tutorial)
