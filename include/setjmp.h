/**
 * @file setjmp.h
 * @brief Declares C89 non-local control transfer facilities.
 */

#ifndef WCRT_SETJMP_H
#define WCRT_SETJMP_H

#if defined(__aarch64__)
/** @brief Saved Windows ARM64 nonvolatile execution context. */
struct wcrt_jump_context {
    unsigned long long integer[13]; /**< X19-X30 and stack pointer. */
    unsigned long long vector[8];   /**< Low halves of V8 through V15. */
};
#elif defined(__x86_64__)
/** @brief Saved x64 nonvolatile execution context. */
struct wcrt_jump_context {
    unsigned long long integer[10]; /**< Integer context. */
    unsigned char vector[160];      /**< XMM6 through XMM15. */
};
#else
/** @brief Saved x86 nonvolatile execution context. */
struct wcrt_jump_context {
    unsigned long integer[6];       /**< Integer context. */
};
#endif

/** @brief Array type containing one saved execution context. */
typedef struct wcrt_jump_context jmp_buf[1];

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Saves the calling execution environment.
 * @param environment Destination environment.
 * @return Zero directly or the value supplied to longjmp when resumed.
 */
int __wcrt_setjmp(jmp_buf environment);

/**
 * @brief Restores a saved execution environment.
 * @param environment Environment previously populated by setjmp.
 * @param value Value observed from the resumed setjmp; zero becomes one.
 * @note This function does not return directly.
 */
void longjmp(jmp_buf environment, int value);

#ifdef __cplusplus
}
#endif

/** @brief Saves the current execution environment. */
#define setjmp(environment) __wcrt_setjmp(environment)

#endif
