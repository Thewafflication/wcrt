# POSIX compatibility test manifest

| Requirement | Specification | Source | Coverage |
| --- | --- | --- | --- |
| REQ-0050 | [TC-0050](../../docs/tc-0050-posix-selection-errno.tex) | `presence/profile.c`, `absence/profile.c` | Selector, error values, repeated inclusion, C89/C99, strict isolation, and time guard |
| REQ-0051 | [TC-0051](../../docs/tc-0051-posix-stat-utime.tex) | `presence/files.c`, `absence/files.c`, `files.c` | Types, declarations, strict isolation, metadata, time updates, limits, and errors |
| REQ-0052 | [TC-0052](../../docs/tc-0052-posix-dirent.tex) | `presence/dirent.c`, `absence/dirent.c`, `dirent.c` | Declarations, strict isolation, enumeration, rewind, entry types, end of stream, and errors |
| REQ-0058 | [TC-0058](../../docs/tc-0058-posix-strings.tex) | `presence/strings.c`, `absence/strings.c`, `strings.c` | Declarations, strict isolation, C89/C99, ordering, bounds, and zero count |
| REQ-0059 | [TC-0059](../../docs/tc-0059-posix-strdup.tex) | `presence/strdup.c`, `absence/strdup.c`, `strdup.c` | Declaration, strict isolation, C89/C99, content, independence, empty strings, and ownership |
| REQ-0060 | [TC-0060](../../docs/tc-0060-posix-getpid.tex) | `presence/unistd.c`, `absence/unistd.c`, `unistd.c` | Type width, declaration, strict isolation, C89/C99, stable positive identity, and imports |
| REQ-0061 | [TC-0061](../../docs/tc-0061-posix-unlink.tex) | `presence/unlink.c`, `absence/unlink.c`, `unlink.c` | Declaration, strict isolation, C89/C99, removal, missing and null paths, and imports |

The selected profile is a bounded Windows portability layer. It does not
define `_POSIX_VERSION`, and the manifest does not imply complete POSIX
conformance.
