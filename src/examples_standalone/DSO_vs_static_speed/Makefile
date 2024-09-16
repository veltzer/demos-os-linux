.PHONY: all
all: main libshared.so

main: main.c static_lib.c
	gcc -O2 -o main main.c static_lib.c -ldl -lrt

libshared.so: shared_lib.c
	gcc -O2 -shared -fPIC -o libshared.so shared_lib.c

.PHONY: clean
clean:
	-rm -f main libshared.so
