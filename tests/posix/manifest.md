# POSIX compatibility test manifest

| Requirement | Specification | Source | Coverage |
| --- | --- | --- | --- |
| REQ-0050 | [TC-0050](../../docs/tc-0050-posix-selection-errno.tex) | `presence/profile.c`, `absence/profile.c` | Selector, error values, repeated inclusion, C89/C99, strict isolation, and time guard |
| REQ-0051 | [TC-0051](../../docs/tc-0051-posix-stat-utime.tex) | `presence/files.c`, `absence/files.c`, `files.c` | Types, declarations, strict isolation, metadata, time updates, limits, and errors |
| REQ-0052 | [TC-0052](../../docs/tc-0052-posix-dirent.tex) | `presence/dirent.c`, `absence/dirent.c`, `dirent.c` | Declarations, strict isolation, enumeration, rewind, entry types, end of stream, and errors |

The selected profile is a bounded Windows portability layer. It does not
define `_POSIX_VERSION`, and the manifest does not imply complete POSIX
conformance.
