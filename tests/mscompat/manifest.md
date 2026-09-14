# Microsoft compatibility test manifest

| Requirement | Specification | Source | Coverage |
| --- | --- | --- | --- |
| REQ-0020 | [TC-0020](../../docs/tc-0020-ms-stdio-bounded-format.tex) | `presence/stdio.c`, `stdio.c` | Legacy bounds, termination, null-buffer queries, and return values |
| REQ-0043 | [TC-0043](../../docs/tc-0043-ms-io-header.tex) | `presence/io.c` | Header presence, repeatability, isolation, and controlled surface |
| REQ-0044 | [TC-0044](../../docs/tc-0044-ms-fileno.tex) | `presence/fileno.c`, `fileno.c`, POSIX presence/absence fixtures | Declarations, descriptor and native-handle mapping, stream-state preservation, errors, isolation, and logger compatibility |
| REQ-0045 | [TC-0045](../../docs/tc-0045-ms-case-insensitive-string.tex) | `presence/string-case.c`, `string-case.c` | Ordering, bounds, locale, invalid parameters, and excluded aliases |
| REQ-0046 | [TC-0046](../../docs/tc-0046-ms-secure-stdio.tex) | `presence/secure-stdio.c`, `secure-stdio.c` | File-open errors and secure scanning size contracts |
| REQ-0047 | [TC-0047](../../docs/tc-0047-ms-secure-string-copy.tex) | `presence/secure-string.c`, `secure-string.c` | Copy and concatenation bounds, termination, `_TRUNCATE`, clearing, and errors |
| REQ-0048 | [TC-0048](../../docs/tc-0048-ms-utime.tex) | `presence/utime.c`, `utime.c` | ABI, time ranges, filesystem behavior, errors, and imports |
| REQ-0049 | [TC-0049](../../docs/tc-0049-ms-stat.tex) | `presence/stat.c`, `stat.c` | Layout, path and descriptor metadata, permission changes, x86 aliases, errors, symbols, and imports |
| REQ-0053 | [TC-0053](../../docs/tc-0053-ms-string-transform.tex) | `presence/string-transform.c`, `string-transform.c` | In-place case conversion, reversal, return identity, and null errors |
| REQ-0054 | [TC-0054](../../docs/tc-0054-ms-strdup.tex) | `presence/strdup.c`, `strdup.c` | Allocation, content, independence, empty strings, ownership, and null errors |
| REQ-0055 | [TC-0055](../../docs/tc-0055-ms-string-set.tex) | `presence/string-set.c`, `string-set.c` | Full and bounded replacement, count boundaries, termination, return identity, and null errors |
| REQ-0056 | [TC-0056](../../docs/tc-0056-ms-integer-string.tex) | `presence/integer-string.c`, `integer-string.c` | Radices, signs, integer boundaries, return identity, and invalid parameters |
| REQ-0057 | [TC-0057](../../docs/tc-0057-ms-memory-case.tex) | `presence/memory-case.c`, `memory-case.c` | Case folding, ordering, embedded nulls, bounds, and invalid parameters |
| REQ-0062 | [TC-0062](../../docs/tc-0062-ms-unlink.tex) | `presence/unlink.c`, `unlink.c` | Declaration, removal, missing and null paths, and imports |
| REQ-0063 | [TC-0063](../../docs/tc-0063-ms-access.tex) | `presence/access.c`, `access.c` | Declaration, access modes, invalid mode, missing and null paths, and imports |
| REQ-0064 | [TC-0064](../../docs/tc-0064-ms-mkdir.tex) | `presence/mkdir.c`, `mkdir.c` | Directory creation, current directory, absolute paths, allocation, errors, and imports |
| REQ-0067 | [TC-0067](../../docs/tc-0067-descriptor-io.tex) | `presence/lowio.c`, POSIX `lowio.c` fixtures | Descriptor declarations, shared I/O, duplication, pipes, errors, and isolation |
| REQ-0070 | [TC-0070](../../docs/tc-0070-command-streams.tex) | `presence/popen.c`, POSIX fixtures | Microsoft declarations, command output/input, process status, errors, and imports |

This non-ISO suite compiles against WCRT headers without host standard-library
headers. Every listed requirement has an implemented runner and remains part
of the aggregate regression suite.
