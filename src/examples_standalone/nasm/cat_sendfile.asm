; this is following https://www.youtube.com/watch?v=CZTU_VVkBZE&ab_channel=NirLichtman

global _start

section .text

_start:

; open()
mov rax, 2 ; NR_OPEN
mov rdi, [rsp+16] ; rsp+16 -> first argument; rsp+8 -> program name; rsp -> argc
mov rsi, 0 ; O_RDONLY
syscall
mov r8, rax
cmp r8, 0
jl error

; sendfile()
mov rax, 40 ; NR_SENDFILE
mov rsi, r8 ; r8 is the return value of open(2) before = 3
mov rdi, 1 ; out_fd = stdout
mov rdx, 0 ; offset, this is 0=NULL
mov r10, 4096 ; count
syscall
mov r8, rax
cmp r8, 0
jl error
jmp $

error:
end:
; exit(r8)
mov rax, 60
mov rdi, r8
syscall
