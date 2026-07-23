# C89 test manifest

## API presence suite

| Requirement | Specification | Source | Compile-time coverage |
| --- | --- | --- | --- |
| REQ-0001 | [TC-0001](../../docs/tc-0001-assert.tex) | `presence/assert.c`, `assert.c`, `assert_disabled.c` | Standalone header; enabled/disabled evaluation; re-inclusion; diagnostic and termination |
| REQ-0002 | [TC-0002](../../docs/tc-0002-ctype.tex) | `presence/ctype.c`, `ctype.c` | 13 functions; full byte domain; class and case behavior |
| REQ-0003 | [TC-0003](../../docs/tc-0003-errno.tex) | `presence/errno.c`, `errno.c` | Constants; modifiable stable storage; non-aliasing |
| REQ-0004 | [TC-0004](../../docs/tc-0004-float.tex) | `presence/float.c`, `float.c` | All model macros; range and stored epsilon behavior |
| REQ-0005 | [TC-0005](../../docs/tc-0005-limits.tex) | `presence/limits.c`, `limits.c` | All macros; widths, signedness, and range relationships |
| REQ-0006 | [TC-0006](../../docs/tc-0006-locale.tex) | `presence/locale.c`, `locale.c` | API; C-locale categories, failures, conventions, and ctype interaction |
| REQ-0007 | [TC-0007](../../docs/tc-0007-math.tex) | `presence/math.c`, `math.c` | 22 functions; accuracy, identities, pointers, domain and range errors |
| REQ-0008 | [TC-0008](../../docs/tc-0008-setjmp.tex) | `presence/setjmp.c`, `setjmp.c` | x86, x64, and ARM64 contexts; values, nesting, stack restoration, volatile state |
| REQ-0009 | [TC-0009](../../docs/tc-0009-signal.tex) | `presence/signal.c`, `signal.c`, `signal_default.c` | 2 functions, 6 signals, dispositions, delivery, reset, invalid requests, default termination |
| REQ-0010 | [TC-0010](../../docs/tc-0010-stdarg.tex) | `presence/stdarg.c`, `stdarg.c` | Builtins, promotions, pointer, aggregate, repeated calls, empty tail |
| REQ-0011 | [TC-0011](../../docs/tc-0011-stddef.tex) | `presence/stddef.c`, `stddef.c` | ABI types, signedness, nulls, pointer subtraction, nested constant offsets |
| REQ-0012 | [TC-0012](../../docs/tc-0012-stdio.tex) | `presence/stdio.c` | 41 functions, 3 types, 13 macros, 3 streams |
| REQ-0013 | [TC-0013](../../docs/tc-0013-stdlib.tex) | `presence/stdlib.c` | 29 functions, 4 types, 5 macros |
| REQ-0014 | [TC-0014](../../docs/tc-0014-string.tex) | `presence/string.c` | 22 functions, `size_t`, `NULL` |
| REQ-0015 | [TC-0015](../../docs/tc-0015-time.tex) | `presence/time.c` | 9 functions, 3 types, 2 macros, `struct tm` and all members |

These tests establish declaration presence only. Behavioral, link, ABI, error,
boundary, and platform tests remain future work.

## Source quality suite

| Requirement | Specification | Runner | Static coverage |
| --- | --- | --- | --- |
| REQ-0016 | [TC-0016](../../docs/tc-0016-source-documentation.tex) | `run-tc-0016.ps1` | Doxygen documentation and 80-character physical-line limit |
