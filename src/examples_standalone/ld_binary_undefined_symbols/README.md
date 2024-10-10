This is an example of how to link an executable with undefined symbols.

The heart of the idea is to pass '-Xlinker --noinhibit-exec -Xlinker --unresolved-symbols=ignore-all'
to the compiler/linker.
