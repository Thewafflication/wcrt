# C99 test manifest

| Requirement | Specification | Source | Coverage |
| --- | --- | --- | --- |
| REQ-0019 | [TC-0019](../../docs/tc-0019-stdio-bounded-format.tex) | `presence/stdio.c`, `stdio.c` | C99 bounds, termination, length queries, and return values |
| REQ-0021 | [TC-0021](../../docs/tc-0021-stdbool.tex) | Planned: `presence/stdbool.c` | Boolean macros, conversions, and C89 isolation |
| REQ-0022 | [TC-0022](../../docs/tc-0022-stdint.tex) | Planned: `presence/stdint.c` | Integer types, limits, constants, and ABI agreement |
| REQ-0023 | [TC-0023](../../docs/tc-0023-limits-long-long.tex) | Planned: `presence/limits.c` | `long long` limits and C89 regression |
| REQ-0024 | [TC-0024](../../docs/tc-0024-float-c99.tex) | Planned: `float.c` | Floating model and decimal round trips |
| REQ-0025 | [TC-0025](../../docs/tc-0025-va-copy-restrict.tex) | Planned: `va-copy.c` | Independent traversal and declaration audit |
| REQ-0026 | [TC-0026](../../docs/tc-0026-iso646.tex) | Planned: `presence/iso646.c` | Alternative spelling equivalence and isolation |

The suite compiles against WCRT headers without host standard-library headers.
Planning-guard runners fail explicitly until their test sources and conformance
checks are implemented; an indexed requirement is not thereby claimed as met.
