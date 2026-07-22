# C89 test manifest

## API presence suite

| Requirement | Specification | Source | Compile-time coverage |
| --- | --- | --- | --- |
| REQ-0001 | [TC-0001](../../docs/tc-0001-assert.tex) | `presence/assert.c` | `assert`, `NDEBUG` re-inclusion behavior |
| REQ-0002 | [TC-0002](../../docs/tc-0002-ctype.tex) | `presence/ctype.c` | 13 character functions |
| REQ-0003 | [TC-0003](../../docs/tc-0003-errno.tex) | `presence/errno.c` | `errno`, `EDOM`, `ERANGE` |
| REQ-0004 | [TC-0004](../../docs/tc-0004-float.tex) | `presence/float.c` | All C89 floating limit/model macros |
| REQ-0005 | [TC-0005](../../docs/tc-0005-limits.tex) | `presence/limits.c` | All C89 integer/character limit macros |
| REQ-0006 | [TC-0006](../../docs/tc-0006-locale.tex) | `presence/locale.c` | 2 functions, 6 categories, `NULL`, `struct lconv` and all members |
| REQ-0007 | [TC-0007](../../docs/tc-0007-math.tex) | `presence/math.c` | 22 functions and `HUGE_VAL` |
| REQ-0008 | [TC-0008](../../docs/tc-0008-setjmp.tex) | `presence/setjmp.c` | `jmp_buf`, `setjmp`, `longjmp` |
| REQ-0009 | [TC-0009](../../docs/tc-0009-signal.tex) | `presence/signal.c` | 2 functions, `sig_atomic_t`, 3 actions, 6 signals |
| REQ-0010 | [TC-0010](../../docs/tc-0010-stdarg.tex) | `presence/stdarg.c` | `va_list`, `va_start`, `va_arg`, `va_end` |
| REQ-0011 | [TC-0011](../../docs/tc-0011-stddef.tex) | `presence/stddef.c` | 3 types, `NULL`, `offsetof` |
| REQ-0012 | [TC-0012](../../docs/tc-0012-stdio.tex) | `presence/stdio.c` | 41 functions, 3 types, 13 macros, 3 streams |
| REQ-0013 | [TC-0013](../../docs/tc-0013-stdlib.tex) | `presence/stdlib.c` | 29 functions, 4 types, 5 macros |
| REQ-0014 | [TC-0014](../../docs/tc-0014-string.tex) | `presence/string.c` | 22 functions, `size_t`, `NULL` |
| REQ-0015 | [TC-0015](../../docs/tc-0015-time.tex) | `presence/time.c` | 9 functions, 3 types, 2 macros, `struct tm` and all members |

These tests establish declaration presence only. Behavioral, link, ABI, error,
boundary, and platform tests remain future work.
