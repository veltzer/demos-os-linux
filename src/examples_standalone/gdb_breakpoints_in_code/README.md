# Setting `gdb(1)` breakpoints in your code

This example shows how to trigger a breakpoint to a debugger
from your code. This is useful, for instance, when it is hard to express when
you want to break using the debugger API/language.

This example shows that there are several ways by which you can trigger
a debugger interaction:
* `asm("int 3")`
* `raise(SIGTRAP)`
* something illegal?!?
