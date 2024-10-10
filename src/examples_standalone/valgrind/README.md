# Example of how to use `valgrind(1)`

This is an example of how to use valgrind to find memory leaks in your program.

The basic things:
* try to compile with debug information (`-g`) so that you will get source file
    names and line numbers instead of object file names and code segment pointers.
* run `valgrind --tool=memcheck [your_program]`
* you can add `-v` or `--leak-check=full` to get extra info.

References:
[1](http://www.cprogramming.com/debugging/valgrind.html)
