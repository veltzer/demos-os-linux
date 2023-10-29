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
; open()
mov rax, 2
mov rdi, [rsp+16] ; rsp+16 -> first argument; rsp+8 -> program name; rsp -> argc
mov rsi, 0
mov rdx, 0
syscall
; read()
mov rax, 0 ; system call 0 = read
mov rsi, rax
mov rdi, 1
mov rdx, 0
mov r10, 4096
syscall
jmp $
mov rax, 60
mov rdi, 7
syscall
