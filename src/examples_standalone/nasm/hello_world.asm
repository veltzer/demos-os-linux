; this is a comment
; you can find the system call numbers in:
; /usr/include/x86_64-linux-gnu/asm/unistd_64.h
; remember that this is a 64 bit application and system calls numbers are different between linux
; on x32 and x86_64

section .data
msg db "hello, world!", 10  ; 10 is the newline

section .text
global _start
_start:
; write(1,msg,14)
mov rax, 1
mov rdi, 1
mov rsi, msg
mov rdx, 14 ; 14 is the character count in the msg string
; exit(7)
syscall
mov rax, 60
mov rdi, 7
syscall
