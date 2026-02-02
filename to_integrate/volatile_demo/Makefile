main.elf: main.c libcallme.so
	gcc -I../../src/include -O2 main.c -o $@ -L. -lcallme

libcallme.so: callme.o
	gcc -shared -o libcallme.so callme.o

callme.o: callme.c
	gcc -I../../src/include -c -O2 -fPIC callme.c -o callme.o
