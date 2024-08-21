main: main.c libcallme.so
	gcc -O2 main.c -o main -L. -lcallme

libcallme.so: callme.o
	gcc -shared -o libcallme.so callme.o

callme.o: callme.c
	gcc -c -O2 -fPIC callme.c -o callme.o
