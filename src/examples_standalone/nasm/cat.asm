; this is following https://www.youtube.com/watch?v=CZTU_VVkBZE&ab_channel=NirLichtman

global _start

section .text

_start:
mov rax, 2
; rsp+16 -> first argument; rsp+8 -> program name; rsp -> argc
; open()
mov rdi, [rsp+16]
mov rsi, 0
mov rdx, 0
; sendfile()
syscall
mov rsi, rax
mov rdi, 1
mov rax, 40
mov rdx, 0
mov r10, 4096
syscall
jmp $
mov rax, 60
mov rdi, 7
syscall
