include ../../../Makefile.mk

.PHONY: all
all: main.pic.o main.nopic.o

main.pic.o: main.c
	$(info doing [$@])
	$(Q)gcc -c -fPIC -o $@ $<
main.nopic.o: main.c
	$(info doing [$@])
	$(Q)gcc -c -fno-pic -o $@ $<

.PHONY: clean
clean:
	$(info doing [$@])
	$(Q)-rm -f *.o

.PHONY: dis
dis: main.pic.o main.nopic.o
	objdump -j .text --disassemble=main main.pic.o
	objdump -j .text --disassemble=main main.nopic.o
