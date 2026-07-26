# C99 test manifest

| Requirement | Specification | Source | Coverage |
| --- | --- | --- | --- |
| REQ-0019 | [TC-0019](../../docs/tc-0019-stdio-bounded-format.tex) | `presence/stdio.c`, `stdio.c` | C99 bounds, termination, length queries, and return values |

The suite compiles against WCRT headers without host standard-library headers.
Its runner is expected to fail until the proposed requirement is implemented.
