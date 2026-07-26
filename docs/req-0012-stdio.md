# REQ-0012 — `<stdio.h>`

**Content type:** Project requirement

**Status:** Implemented

**Source:** §4.9 Input/output

## Scope

### Required files

- `include/stdio.h` — public stream types, macros, objects, and declarations.
- `src/stdio.c` or `src/stdio/` — stream core and all function families.
- `src/internal/file.h` — private `FILE` representation and invariants.
- `src/platform/windows/file.c` — handle, path, and file operations.
- `src/platform/windows/console.c` — console and standard-stream adaptation.
- `tests/c89/stdio.c` plus focused format, scan, file, and buffering tests.

### Functions

| Draft clause | Required functions |
| --- | --- |
| §4.9.4 File operations | `remove`, `rename`, `tmpfile`, `tmpnam` |
| §4.9.5 File access | `fclose`, `fflush`, `fopen`, `freopen`, `setbuf`, `setvbuf` |
| §4.9.6 Formatted I/O | `fprintf`, `fscanf`, `printf`, `scanf`, `sprintf`, `sscanf`, `vfprintf`, `vprintf`, `vsprintf` |
| §4.9.7 Character I/O | `fgetc`, `fgets`, `fputc`, `fputs`, `getc`, `getchar`, `gets`, `putc`, `putchar`, `puts`, `ungetc` |
| §4.9.8 Direct I/O | `fread`, `fwrite` |
| §4.9.9 File positioning | `fgetpos`, `fseek`, `fsetpos`, `ftell`, `rewind` |
| §4.9.10 Error handling | `clearerr`, `feof`, `ferror`, `perror` |

### Other public surface

Types `FILE`, `fpos_t`, and `size_t`; macros `NULL`, `EOF`, `FOPEN_MAX`,
`FILENAME_MAX`, `BUFSIZ`, `_IOFBF`, `_IOLBF`, `_IONBF`, `SEEK_SET`,
`SEEK_CUR`, `SEEK_END`, `TMP_MAX`, and `L_tmpnam`; and standard streams
`stdin`, `stdout`, and `stderr`.

## Requirement

- Implement §4.9.2 stream and §4.9.3 file semantics: text/binary modes, update
  streams, buffering, indicators, pushback, positioning, and termination flush.
- Parse every C89 formatted-I/O conversion, flag, width, precision, length
  modifier, assignment suppression, and `%n` rule with correct return values.
- `getc` and `putc` macro forms may evaluate their stream arguments as permitted
  by the draft; all other macro evaluation shall meet §4.1.6.
- `gets` is required by C89 and shall be implemented, while documentation shall
  prominently mark it unsafe and obsolete.
- Windows path, newline, console, sharing, temporary-file, and error mappings
  shall be documented. Baseline code shall avoid post-Windows-2000 imports.

## Rationale

WCRT must provide its own `<stdio.h>` contract to supply C89-conforming behavior without depending on a host C runtime.

## Verification

**Method:** Automated test and inspection

**References:** `TC-0012`

Tests shall cover every function and conversion, boundary sizes, short I/O,
EOF/error distinctions, buffering modes, update sequencing, text/binary
translation, positioning, temporary files, standard streams, and cleanup.
Shared gates apply.

## Relationships

- **Derived from:** §4.9 Input/output
- **Depends on:** Shared build, ABI, and quality gates in `docs/REQUIREMENTS.md`
- **Conflicts with:** None known

## Tailoring

WCRT groups the related obligations for this C89 conformance unit under one
stable requirement identifier. Each observable obligation remains explicit in
this record and is verified collectively by `TC-0012`.

## Implementation Record

Streams use Windows handles and Windows 2000-era file APIs without host CRT
delegation. Text streams translate CR-LF on input and output; binary streams
preserve bytes. A stream records requested full, line, or unbuffered mode;
transfers currently use immediate Windows I/O, so `fflush` has no pending WCRT
bytes to commit. Temporary streams use the Windows temporary directory and are
deleted at close. `gets` is present only for C89 compatibility and is unsafe;
new code must use `fgets`.

Formatting and scanning share internal parsers covering C89 flags, dynamic and
literal widths, precision, length modifiers, assignment suppression, scansets,
and `%n`. Stream scanning uses a bounded 4096-byte staging record. This limit
is an implementation constraint to remove in a later hardening pass.
