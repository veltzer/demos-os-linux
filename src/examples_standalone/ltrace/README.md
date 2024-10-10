This example demonstrates how to use ltrace(1) to trace your own functions.

just do "make trace" to see the trace.

do "make trace_only_func" to trace only the function 'func' from our
own 'libadd.so' DSO.

to trace an app using ltrace:
	$ ltrace [yourapp.elf]

the problem is that you are tracing ALL library calls (including glibc and
any other library that you are using). In order to trace a single
function use:
	$ ltrace -e [funcname] [yourapp.elf]
