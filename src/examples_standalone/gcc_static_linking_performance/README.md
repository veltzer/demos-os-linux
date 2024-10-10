This example tries to answer the question of how much more performance
you gain when you create a static version of your library and link to
that as opposed to creating a dynamic version of your library and
dynamic linking with that.

The answer is quite a bit...:)

run 'make time' to see the difference between calling a billion functions
statically vs dynamically.

Where does this difference come from? Two sources.
- the -fpic flag passed to the compiler when compiling the compilation units
of the dynamic library forces constrains on the compiler which create slower
code. Compiler needs to always code relative jumps instead of absolute ones. 
On some architectures (like i386) this is expensive and requires several
machine instruction per jump. On some architectures (like ia64) this is
much less expensive since the architecture has a fast single instruction
'relative jump' instruction.
- the extra dereference (the GOT and PLT) which needs to be used to access
either data or functions in the dynamic link case. This means that the
cost of calling a function from a dynamic library is similar to the cost
of calling a virtual method in C++.
