# Explore how GCC creates target files

This example shows that gcc unlinks an object file before it creates it.
You can see this by the fact that the two object files are not identical
after the build...

Why is this important? It allows you to create hard links for identical
targets and not worry about gcc/g++ overwriting both copies of the file.
