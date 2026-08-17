/*
 * TinyCC ARM64 complex-arithmetic helper ABI bridges.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#if defined(__aarch64__)

    .text

/*
 * TinyCC 0.9.28-rc.1442 emits helper calls with the four scalar components
 * in x0-x3 and the result pointer in x4.  Its C-compiled helper entry points
 * expect the components in d0-d3 and the pointer in x0.  Normalize that
 * compiler-private boundary before tail-calling the scalar WCRT helpers.
 */
    .globl __tcc_muldc3
__tcc_muldc3:
    .long 0x9e670000 /* fmov d0, x0 */
    .long 0x9e670021 /* fmov d1, x1 */
    .long 0x9e670042 /* fmov d2, x2 */
    .long 0x9e670063 /* fmov d3, x3 */
    mov x0, x4
    b __wcrt_tinycc_muldc3

    .globl __tcc_divdc3
__tcc_divdc3:
    .long 0x9e670000 /* fmov d0, x0 */
    .long 0x9e670021 /* fmov d1, x1 */
    .long 0x9e670042 /* fmov d2, x2 */
    .long 0x9e670063 /* fmov d3, x3 */
    mov x0, x4
    b __wcrt_tinycc_divdc3

/* Windows ARM64 long double has the binary64 representation and ABI. */
    .globl __tcc_mulxc3
__tcc_mulxc3:
    b __tcc_muldc3

    .globl __tcc_divxc3
__tcc_divxc3:
    b __tcc_divdc3

#endif
