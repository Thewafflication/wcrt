# C89 test manifest

## API presence suite

| Requirement | Test case | Compile-time coverage |
| --- | --- | --- |
| REQ-0001 | `presence/assert.c` | `assert`, `NDEBUG` re-inclusion behavior |
| REQ-0002 | `presence/ctype.c` | 13 character functions |
| REQ-0003 | `presence/errno.c` | `errno`, `EDOM`, `ERANGE` |
| REQ-0004 | `presence/float.c` | All C89 floating limit/model macros |
| REQ-0005 | `presence/limits.c` | All C89 integer/character limit macros |
| REQ-0006 | `presence/locale.c` | 2 functions, 6 categories, `NULL`, `struct lconv` and all members |
| REQ-0007 | `presence/math.c` | 22 functions and `HUGE_VAL` |
| REQ-0008 | `presence/setjmp.c` | `jmp_buf`, `setjmp`, `longjmp` |
| REQ-0009 | `presence/signal.c` | 2 functions, `sig_atomic_t`, 3 actions, 6 signals |
| REQ-0010 | `presence/stdarg.c` | `va_list`, `va_start`, `va_arg`, `va_end` |
| REQ-0011 | `presence/stddef.c` | 3 types, `NULL`, `offsetof` |
| REQ-0012 | `presence/stdio.c` | 41 functions, 3 types, 13 macros, 3 streams |
| REQ-0013 | `presence/stdlib.c` | 29 functions, 4 types, 5 macros |
| REQ-0014 | `presence/string.c` | 22 functions, `size_t`, `NULL` |
| REQ-0015 | `presence/time.c` | 9 functions, 3 types, 2 macros, `struct tm` and all members |

These tests establish declaration presence only. Behavioral, link, ABI, error,
boundary, and platform tests remain future work.
