/* REQ-0008: <setjmp.h> presence. Compile only; do not link. */
#include <setjmp.h>

#ifndef setjmp
/* A function form is permitted, so declaration use below remains the check. */
#endif

typedef void (*wcrt_longjmp_fn)(jmp_buf, int);

static jmp_buf wcrt_jump_buffer;
static wcrt_longjmp_fn wcrt_longjmp_address = (longjmp);

static int wcrt_setjmp_is_callable(void)
{
    return setjmp(wcrt_jump_buffer);
}
