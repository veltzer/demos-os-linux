This example explores reverse debugging in gdb(1).

In order to do reverse debugging:
- compile the program with debug information.
- run gdb:
	shell$ gdb main.elf
- set a break point where you want recording to start say:
	(gdb) break main
- now start recording:
	(gdb) target record-full
- set another breakpoint where the bug happens:
	(gdb) break main.cc:28
- now get to that break point:
	(gdb) continue
	or just
	(gdb) c
- now back up using reverse-* commands:
	(gdb) reverse-next
	or just:
	(gdb) rn

References:
http://www.gnu.org/software/gdb/news/reversible.html
http://stackoverflow.com/questions/1470434/how-does-reverse-debugging-work
https://www.sourceware.org/gdb/wiki/ProcessRecord/Tutorial
