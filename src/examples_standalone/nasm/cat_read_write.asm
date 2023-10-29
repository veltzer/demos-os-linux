; this demo is of my own making and it's cat implemented with regular
; read(2) and write(2) system calls
; you can find the system call numbers in:
; /usr/include/x86_64-linux-gnu/asm/unistd_64.h

global _start

section .data

; Reserve 4096 bytes for my_buffer
my_buffer times 4096 db 0

section .text

_start:
; open(2)
mov rax, 2 ; NR_open
mov rdi, [rsp+16] ; rsp+16 -> first argument; rsp+8 -> program name; rsp -> argc
mov rsi, 0 ; O_RDONLY
syscall
mov r8, rax
cmp r8, 0
jl error
mov r9, r8

read:
; read(2)
mov rax, 0 ; NR_read
mov rdi, r9 ; r9 is the return value of open(2) before = 3
mov rsi, my_buffer
mov rdx, 4096
syscall
mov r8, rax
cmp r8, 0
jle error

; write(2)
mov rax, 1 ; NR_write
mov rdi, 1 ; STDOUT_FILENO
mov rsi, my_buffer ; the buffer to write(2)
mov rdx, r8 ; size of the buffer which is the return value of the previous read(2)
syscall
mov r8, rax
cmp r8, 0
jl error

jmp read 

error:
end:
; exit(r8)
mov rax, 60
mov rdi, r8
syscall
