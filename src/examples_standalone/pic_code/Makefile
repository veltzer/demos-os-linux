include ../../../Makefile.mk

.PHONY: all
all: main.pic.o main.nopic.o

main.pic.o: main.c
	gcc -c -fPIC -o $@ $<
main.nopic.o: main.c
	gcc -c -fno-pic -o $@ $<

.PHONY: clean
clean:
	rm -f *.o

.PHONY: dis
dis: main.pic.o main.nopic.o
	objdump -j .text --disassemble=main main.pic.o
	objdump -j .text --disassemble=main main.nopic.o
