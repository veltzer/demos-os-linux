.PHONY: all
all: main.elf libshared.so

main.elf: main.c static_lib.c
	gcc -O2 -o $@ main.c static_lib.c -ldl -lrt

libshared.so: shared_lib.c
	gcc -O2 -shared -fPIC -o libshared.so shared_lib.c

.PHONY: clean
clean:
	-rm -f main.elf libshared.so
