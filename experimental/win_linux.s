.extern printf
.global main

.section .data
msg:
    .string "Hello from Windows using Linux ABI!\n"

.section .text
main:
    pushq   %rbp
    movq    %rsp, %rbp
    subq    $32, %rsp
    
    leaq    msg(%rip), %rcx
    call    printf
    
    xorl    %eax, %eax
    leave
    ret