all: restrict_opt.elf restrict_dbg.elf

res_opt.o: res.c Makefile
	gcc -I ../../src/include -c -g3 --std=c99 -O3 -o $@ $<
res_dbg.o: res.c Makefile
	gcc -I ../../src/include -c -g3 --std=c99 -O0 -o $@ $<
restrict_opt.elf: restrict.c res_opt.o Makefile
	gcc -I ../../src/include --std=c99 -O3 -o $@ restrict.c res_opt.o
restrict_dbg.elf: restrict.c res_dbg.o Makefile
	gcc -I ../../src/include --std=c99 -O0 -o $@ restrict.c res_dbg.o

clean:
	rm restrict_opt restrict_dbg
