; this is following https://www.youtube.com/watch?v=CZTU_VVkBZE&ab_channel=NirLichtman

global _start

section .text

_start:

; open()
mov rax, 2 ; NR_OPEN
mov rdi, [rsp+16] ; rsp+16 -> first argument; rsp+8 -> program name; rsp -> argc
mov rsi, 0 ; O_RDONLY
syscall

; sendfile()
mov rsi, rax ; rax is the return value of open(2) before = 3
mov rax, 40 ; NR_SENDFILE
mov rdi, 1 ; out_fd = stdout
mov rdx, 0 ; offset, this is 0=NULL
mov r10, 4096 ; count
syscall
jmp $

; exit(7)
mov rax, 60
mov rdi, 7
syscall
