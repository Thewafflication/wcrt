# C99 test manifest

| Requirement | Specification | Source | Coverage |
| --- | --- | --- | --- |
| REQ-0019 | [TC-0019](../../docs/tc-0019-stdio-bounded-format.tex) | `presence/stdio.c`, `stdio.c` | C99 bounds, termination, length queries, and return values |
| REQ-0021 | [TC-0021](../../docs/tc-0021-stdbool.tex) | `presence/stdbool.c`, `stdbool.c` | Boolean macros and conversions |
| REQ-0022 | [TC-0022](../../docs/tc-0022-stdint.tex) | `presence/stdint.c`, `stdint.c` | Integer types, limits, constants, and ABI agreement |
| REQ-0023 | [TC-0023](../../docs/tc-0023-limits-long-long.tex) | `presence/limits.c`, `limits.c` | `long long` limits and C89 regression |
| REQ-0024 | [TC-0024](../../docs/tc-0024-float-c99.tex) | `presence/float.c`, `float.c` | Floating-model declarations and behavior |
| REQ-0025 | [TC-0025](../../docs/tc-0025-va-copy-restrict.tex) | `presence/stdarg.c`, `presence/restrict-c89.c`, `va-copy.c`, `Verify-RestrictDeclarations.ps1` | Independent traversal, declaration audit, and C89 isolation |
| REQ-0026 | [TC-0026](../../docs/tc-0026-iso646.tex) | `presence/iso646.c`, `iso646.c` | Alternative spelling equivalence |

The suite compiles against WCRT headers without host standard-library headers.
TC-0021 through TC-0026 are implemented test cases. They are expected to fail
until their corresponding C99 library interfaces are implemented.
