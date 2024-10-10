# `printf(3)` is a macro

This example shows that `printf(3)` is actually a macro in current glibc versions.

You can see this by analyzing the binary creates and seeing that a call to `printf(3)`
is nowhere to be found and is replaced by a call to `puts(3)`.
