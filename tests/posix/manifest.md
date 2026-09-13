# POSIX compatibility test manifest

| Requirement | Specification | Source | Coverage |
| --- | --- | --- | --- |
| REQ-0050 | [TC-0050](../../docs/tc-0050-posix-selection-errno.tex) | `presence/profile.c`, `absence/profile.c` | Selector, POSIX and shared filesystem error values, repeated inclusion, C89/C99, strict isolation, and time guard |
| REQ-0051 | [TC-0051](../../docs/tc-0051-posix-stat-utime.tex) | `presence/files.c`, `absence/files.c`, `files.c` | Types, path/link/descriptor metadata, identity, predicates, permission and time updates, isolation, and errors |
| REQ-0052 | [TC-0052](../../docs/tc-0052-posix-dirent.tex) | `presence/dirent.c`, `absence/dirent.c`, `dirent.c` | Entry layout, enumeration, descriptors, positioning, scanning, sorting, ownership, isolation, and errors |
| REQ-0058 | [TC-0058](../../docs/tc-0058-posix-strings.tex) | `presence/strings.c`, `absence/strings.c`, `strings.c` | Declarations, strict isolation, C89/C99, ordering, bounds, and zero count |
| REQ-0059 | [TC-0059](../../docs/tc-0059-posix-strdup.tex) | `presence/strdup.c`, `absence/strdup.c`, `strdup.c` | `strdup` and `strndup` declarations, strict isolation, C89/C99, bounded content, independence, empty strings, and ownership |
| REQ-0060 | [TC-0060](../../docs/tc-0060-posix-getpid.tex) | `presence/unistd.c`, `absence/unistd.c`, `unistd.c` | Type width, declaration, strict isolation, C89/C99, stable positive identity, and imports |
| REQ-0061 | [TC-0061](../../docs/tc-0061-posix-unlink.tex) | `presence/unlink.c`, `absence/unlink.c`, `unlink.c` | Declaration, strict isolation, C89/C99, removal, missing and null paths, and imports |
| REQ-0065 | [TC-0065](../../docs/tc-0065-posix-access-mkdir.tex) | `presence/paths.c`, `absence/paths.c`, `paths.c` | Path limits, access, creation, current and absolute paths, strict isolation, allocation, and errors |
| REQ-0066 | [TC-0066](../../docs/tc-0066-posix-gettimeofday.tex) | `presence/timeval.c`, `absence/timeval.c`, `timeval.c` | Wall-clock seconds and microseconds, strict isolation, C89/C99, and errors |
| REQ-0067 | [TC-0067](../../docs/tc-0067-descriptor-io.tex) | Microsoft and POSIX `lowio.c` fixtures | Open flags, shared stream/descriptor data and duplicate offsets, independent closure, isolation, and errors |

The selected profile is a bounded Windows portability layer. It does not
define `_POSIX_VERSION`, and the manifest does not imply complete POSIX
conformance.
