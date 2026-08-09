# C99 test manifest

| Requirement | Specification | Source | Coverage |
| --- | --- | --- | --- |
| REQ-0019 | [TC-0019](../../docs/tc-0019-stdio-bounded-format.tex) | `presence/stdio.c`, `stdio.c` | C99 bounds, termination, length queries, and return values |
| REQ-0021 | [TC-0021](../../docs/tc-0021-stdbool.tex) | `presence/stdbool.c`, `presence/stdbool-c89.c`, `stdbool.c` | Boolean macros, conversions, and C89 isolation |
| REQ-0022 | [TC-0022](../../docs/tc-0022-stdint.tex) | `presence/stdint.c`, `presence/stdint-c89.c`, `stdint.c` | Integer types, limits, constants, ABI agreement, and C89 isolation |
| REQ-0023 | [TC-0023](../../docs/tc-0023-limits-long-long.tex) | `presence/limits.c`, `presence/limits-c89.c`, `limits.c` | `long long` limits and C89 regression |
| REQ-0024 | [TC-0024](../../docs/tc-0024-float-c99.tex) | `presence/float.c`, `presence/float-c89.c`, `float.c` | Floating-model declarations, behavior, analysis, and C89 isolation |
| REQ-0025 | [TC-0025](../../docs/tc-0025-va-copy-restrict.tex) | `presence/stdarg.c`, `presence/restrict-c89.c`, `va-copy.c`, `Verify-RestrictDeclarations.ps1` | Independent traversal, declaration audit, and C89 isolation |
| REQ-0026 | [TC-0026](../../docs/tc-0026-iso646.tex) | `presence/iso646.c`, `presence/iso646-c89.c`, `iso646.c` | Alternative spelling equivalence and C89 isolation |
| REQ-0027 | [TC-0027](../../docs/tc-0027-inttypes.tex) | `presence/inttypes.c`, `presence/inttypes-c89.c`, `inttypes.c` | Types, all format macro families, arithmetic, conversions, errors, ABI, and C89 isolation |
| REQ-0028 | [TC-0028](../../docs/tc-0028-stdlib-c99.tex) | `presence/stdlib.c`, `presence/stdlib-c89.c`, `stdlib.c`, `stdlib-exit.c` | C99 utilities, integer and decimal floating conversions, `_Exit`, errors, ABI, and C89 regression |
| REQ-0029 | [TC-0029](../../docs/tc-0029-stdio-c99-format.tex) | `presence/stdio-t2.c`, `presence/stdio-t2-c89.c`, `stdio-format.c` | Modifiers, output conversions, fields, character bridge, sinks, and contract separation |
| REQ-0030 | [TC-0030](../../docs/tc-0030-stdio-c99-scan.tex) | `presence/stdio-t2.c`, `presence/stdio-t2-c89.c`, `stdio-scan.c` | Modifiers, input conversions, failures, assignments, retained input, and v-list functions |
| REQ-0039 | [TC-0039](../../docs/tc-0039-numeric-text.tex) | `presence/stdlib.c`, `presence/stdlib-c89.c`, `numeric-text.c` | Hexadecimal floating text, classification, signed zero, rounding, range, and round trips |

The suite compiles against WCRT headers without host standard-library headers.
Every row is controlled by `tools/run-extension-tests.ps1`; the runner rejects
an inventory that differs from this manifest, records every case in the
architecture-specific aggregate JSON, and fails when any controlled case does
not pass.
