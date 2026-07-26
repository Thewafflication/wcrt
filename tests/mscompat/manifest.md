# Microsoft compatibility test manifest

| Requirement | Specification | Source | Coverage |
| --- | --- | --- | --- |
| REQ-0020 | [TC-0020](../../docs/tc-0020-ms-stdio-bounded-format.tex) | `presence/stdio.c`, `stdio.c` | Legacy bounds, termination, null-buffer queries, and return values |

This non-ISO suite compiles against WCRT headers without host
standard-library headers. Its runner is expected to fail until the proposed
compatibility requirement is implemented.
