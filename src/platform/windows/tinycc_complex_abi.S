/*
 * TinyCC ARM64 complex-arithmetic helper ABI bridges.
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#if defined(__aarch64__)

    .text

/*
 * Current TinyCC declares the compiler-private helpers as standard Windows
 * ARM64 functions: the four scalar components arrive in d0-d3 and the result
 * pointer arrives in x0.  Preserve that ABI and tail-call the scalar WCRT
 * implementations under their project-owned names.
 */
    .globl __tcc_muldc3
__tcc_muldc3:
    b __wcrt_tinycc_muldc3

    .globl __tcc_divdc3
__tcc_divdc3:
    b __wcrt_tinycc_divdc3

/* Windows ARM64 long double has the binary64 representation and ABI. */
    .globl __tcc_mulxc3
__tcc_mulxc3:
    b __tcc_muldc3

    .globl __tcc_divxc3
__tcc_divxc3:
    b __tcc_divdc3

#endif
