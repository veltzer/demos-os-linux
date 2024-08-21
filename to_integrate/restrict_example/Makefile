all: restrict_opt restrict_dbg

res_opt.o: res.c Makefile
	gcc -c -g3 --std=c99 -O3 -o $@ $<
res_dbg.o: res.c Makefile
	gcc -c -g3 --std=c99 -O0 -o $@ $<
restrict_opt: restrict.c res_opt.o Makefile
	gcc --std=c99 -O3 -o $@ restrict.c res_opt.o
restrict_dbg: restrict.c res_dbg.o Makefile
	gcc --std=c99 -O0 -o $@ restrict.c res_dbg.o

clean:
	rm restrict_opt restrict_dbg
