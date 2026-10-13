# Microsoft compatibility test manifest

| Requirement | Specification | Source | Coverage |
| --- | --- | --- | --- |
| REQ-0103 | [TC-0103](../../docs/tc-0103-ms-sentinel-copy.tex) | `presence/sentinel-copy.c`, `sentinel-copy.c`, `sentinel-copy-main.c` | All byte values, bounded stops, converted sentinels, guards and return pointers |
| REQ-0095 | [TC-0095](../../docs/tc-0095-vc6-calendar-time32.tex) | `presence/calendar-time32.c`, `calendar-time32.c`, `vc6-calendar-time32.c`, `calendar-clock-errors.c` | Narrow calendar source ABI, guards/bounds, normalization/static storage and deterministic epoch/current-time providers |
| REQ-0096 | [TC-0096](../../docs/tc-0096-vc6-find32.tex) | `presence/find32.c`, `find32.c`, `find32-errors.c`, `vc6-find32.c` | Narrow records/source selection, native searches, overflow, guards and handle cleanup |
| REQ-0097 | [TC-0097](../../docs/tc-0097-ms-byte-swap.tex) | `presence/byte-swap.c`, `byte-swap.c`, `byte-swap-main.c` | Byte-pair copying, guards, odd tails, exact in-place copying and invalid inputs |
| REQ-0098 | [TC-0098](../../docs/tc-0098-ms-descriptor-locking.tex) | `presence/descriptor-locking.c`, `descriptor-locking.c`, `descriptor-locking-errors.c` | Exact ranges, wide positions, native contention, state retention and deterministic retries/errors |
| REQ-0099 | [TC-0099](../../docs/tc-0099-ms-descriptor-create.tex) | `presence/descriptor-create.c`, `descriptor-create.c`, `descriptor-create-errors.c` | Create/truncate, read/write text access, sharing, permissions and delegated errors |
| REQ-0100 | [TC-0100](../../docs/tc-0100-vc6-find32i64.tex) | `presence/find32i64.c`, `find32i64.c`, `find32i64-errors.c`, `vc6-find32.c` | Narrow times/wide sizes, mixed records, aliases and failed-next retention |
| REQ-0101 | [TC-0101](../../docs/tc-0101-ms-creation-mask.tex) | `presence/creation-mask.c`, `creation-mask.c`, `creation-mask-errors.c` | Previous masks, ignored bits, all mode partitions and creation-only snapshots |
| REQ-0102 | [TC-0102](../../docs/tc-0102-ms-default-translation.tex) | `presence/default-translation.c`, `default-translation.c`, `default-translation-errors.c`, `vc6-default-mode.c` | Default/explicit modes, physical bytes, snapshots and DLL data writes |
| REQ-0094 | [TC-0094](../../docs/tc-0094-ms-descriptor-resize.tex) | `presence/descriptor-resize.c`, `descriptor-resize.c`, `descriptor-resize-errors.c`, `vc6-headers.c` | Zero-filled extension/truncation, duplicate/wide position and state preservation, modes and native/provider errors |
| REQ-0093 | [TC-0093](../../docs/tc-0093-ms-descriptor-file-times.tex) | `presence/descriptor-file-times.c`, `descriptor-file-times.c`, `utime-errors.c`, `vc6-utime32.c` | Borrowed descriptor timestamps, checked widths/current time, state/position/ownership and native/provider errors |
| REQ-0092 | [TC-0092](../../docs/tc-0092-ms-error-names.tex) | `vc6-ms-errors.c`, `ms-errors.c` | Selected error-name values, strict/POSIX/MS isolation, combined selectors and runtime/DLL errno interaction |
| REQ-0091 | [TC-0091](../../docs/tc-0091-ms-utime32-route.tex) | `presence/utime-default.c`, `vc6-utime32.c`, `utime32-selected.c` | Explicit legacy time-record routing, guards/round trips, wide API preservation and wrong-target rejection |
| REQ-0090 | [TC-0090](../../docs/tc-0090-ms-native-handle-adoption.tex) | `presence/native-handle.c`, `native-handle.c`, `native-handle-errors.c` | Pointer width, transfer/close ownership, access/translation/append, pipes, exhaustion and slot recovery |
| REQ-0089 | [TC-0089](../../docs/tc-0089-ms-descriptor-eof.tex) | `presence/descriptor-eof.c`, `descriptor-eof.c`, `descriptor-eof-errors.c` | Physical exact-end equality, independent FILE EOF state, stream/position preservation and query failures |
| REQ-0088 | [TC-0088](../../docs/tc-0088-ms-descriptor-size-position.tex) | `presence/descriptor-size-position.c`, `descriptor-size-position.c`, `descriptor-size-errors.c` | Physical sizes, wide positions, queries preserving stream state and size/error boundaries |
| REQ-0087 | [TC-0087](../../docs/tc-0087-ms-system-types.tex) | `presence/system-types.c`, `system-types.c` | Microsoft type identity, signedness, include order and preservation of POSIX widths |
| REQ-0086 | [TC-0086](../../docs/tc-0086-ms-stat32-route.tex) | `presence/stat32.c`, `stat32.c`, `stat32-errors.c`, `stat32-selected.c`, `vc6-stat32.c` | Versioned narrow metadata, exact layout/packing, conversion errors, guarded selector and ABI preservation |
| REQ-0085 | [TC-0085](../../docs/tc-0085-ms-drive-directory.tex) | `presence/drive-directory.c`, `drive-directory.c`, `drive-directory-errors.c` | Selected drive changes/queries, buffers, OS/allocation errors and cwd restoration |
| REQ-0084 | [TC-0084](../../docs/tc-0084-ms-drive-query.tex) | `presence/drive-query.c`, `drive-query.c`, `drive-query-errors.c` | Drive numbers/masks, native agreement, prefix/buffer errors and unchanged cwd |
| REQ-0083 | [TC-0083](../../docs/tc-0083-ms-shared-descriptor.tex) | `presence/shared-descriptor.c`, `shared-descriptor.c`, `shared-descriptor-errors.c` | Shared descriptors, creation permissions, modes, duplicate lifetime, errors and cleanup |
| REQ-0082 | [TC-0082](../../docs/tc-0082-ms-descriptor-mode.tex) | `presence/descriptor-mode.c`, `descriptor-mode.c` | Translation, prior modes, position, duplicates, FILE, pipes, standard handles and errors |
| REQ-0081 | [TC-0081](../../docs/tc-0081-ms-shared-stream.tex) | `presence/shared-stream.c`, `shared-stream.c` | Shared stream modes, access matrix, ordinary opens, duplication lifetime, errors and cleanup |
| REQ-0080 | [TC-0080](../../docs/tc-0080-ms-directory-change.tex) | `presence/directory-change.c`, `directory-change.c`, `directory-errors.c` | Directory change/removal, cwd restoration, root/current protection, OS error mapping and invalid arguments |
| REQ-0079 | [TC-0079](../../docs/tc-0079-vc6-consumer-headers.tex) | `vc6-headers.c`, `vc6-startup.c`, `vc6-macros.c`, `macro-headers.c`, `vc6-c89-calls.c`, `c89-calls.c`, `run-tc-0079.ps1` | Selected headers/macros and C89 call ABI; optional Microsoft /Od and /O2 DLL consumers with input/tool hashes; genuine qualification separately reported |
| REQ-0078 | [TC-0078](../../docs/tc-0078-ms-integer64.tex) | `presence/integer64.c`, `integer64.c` | Microsoft 64-bit conversions, exact radix vectors, limits, boundaries, stopping pointers and errors; VC6 qualification Unknown |
| REQ-0077 | [TC-0077](../../docs/tc-0077-ms-i64-format.tex) | `presence/i64-format.c`, `i64-format.c` | I64 integer output/input, boundaries, varargs, counts, suppression, stream retention; genuine VC6 qualification Unknown |
| REQ-0076 | [TC-0076](../../docs/tc-0076-ms-path-components.tex) | `presence/path-components.c`, `path-components.c` | Path constants, construction, splitting, optional components, boundaries and errors; genuine VC6 qualification Unknown |
| REQ-0020 | [TC-0020](../../docs/tc-0020-ms-stdio-bounded-format.tex) | `presence/stdio.c`, `stdio.c` | Legacy bounds, termination, null-buffer queries, and return values |
| REQ-0043 | [TC-0043](../../docs/tc-0043-ms-io-header.tex) | `presence/io.c` | Header presence, repeatability, isolation, and controlled surface |
| REQ-0044 | [TC-0044](../../docs/tc-0044-ms-fileno.tex) | `presence/fileno.c`, `fileno.c`, POSIX presence/absence fixtures | Declarations, descriptor and native-handle mapping, stream-state preservation, errors, isolation, and logger compatibility |
| REQ-0045 | [TC-0045](../../docs/tc-0045-ms-case-insensitive-string.tex) | `presence/string-case.c`, `string-case.c` | Ordering, bounds, locale, invalid parameters, and excluded aliases |
| REQ-0046 | [TC-0046](../../docs/tc-0046-ms-secure-stdio.tex) | `presence/secure-stdio.c`, `secure-stdio.c` | File-open errors and secure scanning size contracts |
| REQ-0047 | [TC-0047](../../docs/tc-0047-ms-secure-string-copy.tex) | `presence/secure-string.c`, `secure-string.c` | Copy and concatenation bounds, termination, `_TRUNCATE`, clearing, and errors |
| REQ-0048 | [TC-0048](../../docs/tc-0048-ms-utime.tex) | `presence/utime.c`, `utime.c`, `utime-errors.c` | ABI, explicit/current time ranges, fractional last-second clocks, filesystem behavior, provider errors and imports |
| REQ-0049 | [TC-0049](../../docs/tc-0049-ms-stat.tex) | `presence/stat.c`, `stat.c` | Layout, path and descriptor metadata, permission changes, x86 aliases, errors, symbols, and imports |
| REQ-0053 | [TC-0053](../../docs/tc-0053-ms-string-transform.tex) | `presence/string-transform.c`, `string-transform.c` | In-place case conversion, reversal, return identity, and null errors |
| REQ-0054 | [TC-0054](../../docs/tc-0054-ms-strdup.tex) | `presence/strdup.c`, `strdup.c` | Allocation, content, independence, empty strings, ownership, and null errors |
| REQ-0055 | [TC-0055](../../docs/tc-0055-ms-string-set.tex) | `presence/string-set.c`, `string-set.c` | Full and bounded replacement, count boundaries, termination, return identity, and null errors |
| REQ-0056 | [TC-0056](../../docs/tc-0056-ms-integer-string.tex) | `presence/integer-string.c`, `integer-string.c` | Radices, signs, integer boundaries, return identity, and invalid parameters |
| REQ-0057 | [TC-0057](../../docs/tc-0057-ms-memory-case.tex) | `presence/memory-case.c`, `memory-case.c` | Case folding, ordering, embedded nulls, bounds, and invalid parameters |
| REQ-0062 | [TC-0062](../../docs/tc-0062-ms-unlink.tex) | `presence/unlink.c`, `unlink.c` | Declaration, removal, missing and null paths, and imports |
| REQ-0063 | [TC-0063](../../docs/tc-0063-ms-access.tex) | `presence/access.c`, `access.c` | Declaration, access modes, invalid mode, missing and null paths, and imports |
| REQ-0064 | [TC-0064](../../docs/tc-0064-ms-mkdir.tex) | `presence/mkdir.c`, `mkdir.c` | Directory creation, current directory, absolute paths, allocation, errors, and imports |
| REQ-0067 | [TC-0067](../../docs/tc-0067-descriptor-io.tex) | `presence/lowio.c`, `seek-errors.c`, POSIX `lowio.c` fixtures | Descriptor declarations, shared I/O, duplication, pipes, isolation and seek boundaries/errors including sentinel low words |
| REQ-0070 | [TC-0070](../../docs/tc-0070-command-streams.tex) | `presence/popen.c`, POSIX fixtures | Microsoft declarations, command output/input, process status, errors, and imports |

This non-ISO suite compiles against WCRT headers without host standard-library
headers. Every listed requirement has an implemented runner and remains part
of the aggregate regression suite.
