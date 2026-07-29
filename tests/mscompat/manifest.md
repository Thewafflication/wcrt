# Microsoft compatibility test manifest

| Requirement | Specification | Source | Coverage |
| --- | --- | --- | --- |
| REQ-0020 | [TC-0020](../../docs/tc-0020-ms-stdio-bounded-format.tex) | `presence/stdio.c`, `stdio.c` | Legacy bounds, termination, null-buffer queries, and return values |
| REQ-0043 | [TC-0043](../../docs/tc-0043-ms-io-header.tex) | `presence/io.c` | Header presence, repeatability, isolation, and controlled surface |
| REQ-0044 | [TC-0044](../../docs/tc-0044-ms-fileno.tex) | `presence/fileno.c`, `fileno.c` | Declaration, descriptor mapping, stream-state preservation, errors, and logger compatibility |

This non-ISO suite compiles against WCRT headers without host
standard-library headers. Its runner is expected to fail until the proposed
compatibility requirement is implemented.
