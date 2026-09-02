# Microsoft compatibility test manifest

| Requirement | Specification | Source | Coverage |
| --- | --- | --- | --- |
| REQ-0020 | [TC-0020](../../docs/tc-0020-ms-stdio-bounded-format.tex) | `presence/stdio.c`, `stdio.c` | Legacy bounds, termination, null-buffer queries, and return values |
| REQ-0043 | [TC-0043](../../docs/tc-0043-ms-io-header.tex) | `presence/io.c` | Header presence, repeatability, isolation, and controlled surface |
| REQ-0044 | [TC-0044](../../docs/tc-0044-ms-fileno.tex) | `presence/fileno.c`, `fileno.c` | Declaration, descriptor mapping, stream-state preservation, errors, and logger compatibility |
| REQ-0045 | [TC-0045](../../docs/tc-0045-ms-case-insensitive-string.tex) | `presence/string-case.c`, `string-case.c` | Ordering, bounds, locale, invalid parameters, and excluded aliases |
| REQ-0046 | [TC-0046](../../docs/tc-0046-ms-secure-stdio.tex) | `presence/secure-stdio.c`, `secure-stdio.c` | File-open errors and secure scanning size contracts |
| REQ-0047 | [TC-0047](../../docs/tc-0047-ms-secure-string-copy.tex) | `presence/secure-string.c`, `secure-string.c` | Copy and concatenation bounds, termination, `_TRUNCATE`, clearing, and errors |
| REQ-0048 | [TC-0048](../../docs/tc-0048-ms-utime.tex) | `presence/utime.c`, `utime.c` | ABI, time ranges, filesystem behavior, errors, and imports |
| REQ-0049 | [TC-0049](../../docs/tc-0049-ms-stat.tex) | `presence/stat.c`, `stat.c` | Layout, metadata, x86 alias, errors, symbols, and imports |
| REQ-0053 | [TC-0053](../../docs/tc-0053-ms-string-transform.tex) | `presence/string-transform.c`, `string-transform.c` | In-place case conversion, reversal, return identity, and null errors |

This non-ISO suite compiles against WCRT headers without host
standard-library headers. A runner failure is expected while its proposed
compatibility requirement remains unimplemented.
