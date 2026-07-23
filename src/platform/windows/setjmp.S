/*
 * WCRT x86, x64, and ARM64 non-local control transfer.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#if defined(__aarch64__)

    .text
    .globl __wcrt_setjmp
__wcrt_setjmp:
    stp x19, x20, [x0, #0]
    stp x21, x22, [x0, #16]
    stp x23, x24, [x0, #32]
    stp x25, x26, [x0, #48]
    stp x27, x28, [x0, #64]
    str x29, [x0, #80]
    mov x9, sp
    str x9, [x0, #88]
    str x30, [x0, #96]
    stp d8, d9, [x0, #104]
    stp d10, d11, [x0, #120]
    stp d12, d13, [x0, #136]
    stp d14, d15, [x0, #152]
    mov w0, #0
    ret

    .globl longjmp
longjmp:
    cbnz w1, 1f
    mov w1, #1
1:
    ldr x16, [x0, #96]
    ldr x17, [x0, #88]
    ldp d8, d9, [x0, #104]
    ldp d10, d11, [x0, #120]
    ldp d12, d13, [x0, #136]
    ldp d14, d15, [x0, #152]
    ldp x19, x20, [x0, #0]
    ldp x21, x22, [x0, #16]
    ldp x23, x24, [x0, #32]
    ldp x25, x26, [x0, #48]
    ldp x27, x28, [x0, #64]
    ldr x29, [x0, #80]
    mov sp, x17
    mov w0, w1
    br x16

#elif defined(__x86_64__)

    .text
    .globl __wcrt_setjmp
__wcrt_setjmp:
    movq %rbx, 0(%rcx)
    leaq 8(%rsp), %rax
    movq %rax, 8(%rcx)
    movq %rbp, 16(%rcx)
    movq %rsi, 24(%rcx)
    movq %rdi, 32(%rcx)
    movq %r12, 40(%rcx)
    movq %r13, 48(%rcx)
    movq %r14, 56(%rcx)
    movq %r15, 64(%rcx)
    movq (%rsp), %rax
    movq %rax, 72(%rcx)
    movups %xmm6, 80(%rcx)
    movups %xmm7, 96(%rcx)
    .byte 0x44,0x0f,0x11,0x81,0x70,0x00,0x00,0x00
    .byte 0x44,0x0f,0x11,0x89,0x80,0x00,0x00,0x00
    .byte 0x44,0x0f,0x11,0x91,0x90,0x00,0x00,0x00
    .byte 0x44,0x0f,0x11,0x99,0xa0,0x00,0x00,0x00
    .byte 0x44,0x0f,0x11,0xa1,0xb0,0x00,0x00,0x00
    .byte 0x44,0x0f,0x11,0xa9,0xc0,0x00,0x00,0x00
    .byte 0x44,0x0f,0x11,0xb1,0xd0,0x00,0x00,0x00
    .byte 0x44,0x0f,0x11,0xb9,0xe0,0x00,0x00,0x00
    xorl %eax, %eax
    ret

    .globl longjmp
longjmp:
    movq 72(%rcx), %r10
    movl %edx, %eax
    testl %eax, %eax
    jne 1f
    movl $1, %eax
1:
    movups 80(%rcx), %xmm6
    movups 96(%rcx), %xmm7
    .byte 0x44,0x0f,0x10,0x81,0x70,0x00,0x00,0x00
    .byte 0x44,0x0f,0x10,0x89,0x80,0x00,0x00,0x00
    .byte 0x44,0x0f,0x10,0x91,0x90,0x00,0x00,0x00
    .byte 0x44,0x0f,0x10,0x99,0xa0,0x00,0x00,0x00
    .byte 0x44,0x0f,0x10,0xa1,0xb0,0x00,0x00,0x00
    .byte 0x44,0x0f,0x10,0xa9,0xc0,0x00,0x00,0x00
    .byte 0x44,0x0f,0x10,0xb1,0xd0,0x00,0x00,0x00
    .byte 0x44,0x0f,0x10,0xb9,0xe0,0x00,0x00,0x00
    movq 0(%rcx), %rbx
    movq 16(%rcx), %rbp
    movq 24(%rcx), %rsi
    movq 32(%rcx), %rdi
    movq 40(%rcx), %r12
    movq 48(%rcx), %r13
    movq 56(%rcx), %r14
    movq 64(%rcx), %r15
    movq 8(%rcx), %rsp
    jmp *%r10

#else

    .text
    .globl ___wcrt_setjmp
___wcrt_setjmp:
    movl 4(%esp), %eax
    movl %ebx, 0(%eax)
    leal 4(%esp), %edx
    movl %edx, 4(%eax)
    movl %ebp, 8(%eax)
    movl %esi, 12(%eax)
    movl %edi, 16(%eax)
    movl (%esp), %edx
    movl %edx, 20(%eax)
    xorl %eax, %eax
    ret

    .globl _longjmp
_longjmp:
    movl 4(%esp), %ecx
    movl 8(%esp), %eax
    testl %eax, %eax
    jne 1f
    movl $1, %eax
1:
    movl 20(%ecx), %edx
    movl 0(%ecx), %ebx
    movl 8(%ecx), %ebp
    movl 12(%ecx), %esi
    movl 16(%ecx), %edi
    movl 4(%ecx), %esp
    jmp *%edx

#endif
