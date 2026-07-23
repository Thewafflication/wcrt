/**
 * @file setjmp.c
 * @brief Verifies the compile-time C89 setjmp.h interface.
 * @details File-scope declarations are typed API presence probes.
 */
#include <setjmp.h>

#ifndef setjmp
/* A function form is permitted, so declaration use below remains the check. */
#endif

typedef void (*wcrt_longjmp_fn)(jmp_buf, int);

static jmp_buf wcrt_jump_buffer;
static wcrt_longjmp_fn wcrt_longjmp_address = (longjmp);

/**
 * @brief Verifies that setjmp accepts a jmp_buf expression.
 * @return The value produced by setjmp.
 */
static int wcrt_setjmp_is_callable(void)
{
    return setjmp(wcrt_jump_buffer);
}
