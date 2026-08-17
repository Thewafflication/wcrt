# REQ-0031 — C99 wide-character utilities

**Content type:** Project requirement

**Status:** Implemented; exact native Debug x86/x64/ARM64 verification passes

**Source:** ISO/IEC 9899:1999 as corrected through TC3, §7.24 and §7.8.2

## Scope

This requirement supplies the complete C99 `<wchar.h>` library surface and
the two wide greatest-width conversions declared by `<inttypes.h>`. It covers
wide formatted and unformatted I/O, restartable conversion, strings and
memory, numeric conversion, and time formatting. The encoding and stream ABI
are controlled by REQ-0033.

### Required files

- `include/wchar.h` and `include/inttypes.h` — complete declarations, types,
  macros, and C89 isolation.
- Wide conversion, string, formatted-I/O, unformatted-I/O, numeric, and time
  implementation units.
- `tests/c99/wchar.c` and header-presence tests — behavioral, boundary, and
  declaration verification.

## Requirement

- `<wchar.h>` shall expose `wchar_t`, `wint_t`, `mbstate_t`, `WEOF`, `NULL`,
  `size_t`, `FILE`, `tm`, and `va_list` as required, be self-contained and
  repeatable, and keep all C99 additions absent from the selected C89 surface.
- Wide formatted output shall implement `fwprintf`, `wprintf`, `swprintf`,
  `vfwprintf`, `vwprintf`, and `vswprintf`; wide formatted input shall implement
  `fwscanf`, `wscanf`, `swscanf`, `vfwscanf`, `vwscanf`, and `vswscanf`.
  Length modifiers, numeric conversions, `%c`, `%s`, scansets, `%n`, widths,
  precisions, return values, truncation, matching/input failure, and first
  unread input shall follow their C99 wide-function contracts.
- Wide unformatted I/O shall implement `fgetwc`, `getwc`, `getwchar`, `fputwc`,
  `putwc`, `putwchar`, `fgetws`, `fputws`, and `ungetwc`, including EOF/WEOF,
  buffer length, termination, error, pushback, and orientation behavior.
- Conversion support shall implement `btowc`, `wctob`, `mbsinit`, `mbrlen`,
  `mbrtowc`, `wcrtomb`, `mbsrtowcs`, and `wcsrtombs` with the state and locale
  contract of REQ-0033.
- String and memory support shall implement `wcscpy`, `wcsncpy`, `wcscat`,
  `wcsncat`, `wcscmp`, `wcscoll`, `wcsncmp`, `wcsxfrm`, `wcschr`, `wcscspn`,
  `wcspbrk`, `wcsrchr`, `wcsspn`, `wcsstr`, `wcstok`, `wcslen`, `wmemchr`,
  `wmemcmp`, `wmemcpy`, `wmemmove`, and `wmemset`, including zero-length,
  overlap, termination, and stateful-tokenization boundaries.
- Numeric conversion shall implement `wcstod`, `wcstof`, `wcstold`, `wcstol`,
  `wcstoul`, `wcstoll`, and `wcstoull`, plus `wcstoimax` and `wcstoumax` in
  `<inttypes.h>`. Subject sequences, bases, end pointers, range errors, and
  target destination widths shall match the corresponding narrow conversions
  and REQ-0039.
- `wcsftime` shall implement the C99 time-format conversion set supported by
  `strftime`, honor maximum buffer length including the terminator, and return
  zero without writing beyond the destination when the result does not fit.
- All functions shall use checked size arithmetic where allocation or buffer
  sizing is required. Null pointers shall be accepted only where C99 assigns
  them meaning; no fixed internal line or conversion limit may truncate a
  valid operation.
- Existing C89 conversions, byte-oriented formatted I/O, unformatted stream
  I/O, position handling, EOF/error indicators, and startup imports shall
  remain green. The T2 narrow/wide bridge shall be reverified against the
  accepted `wint_t` ABI.

## Rationale

The wide library is one coupled behavioral surface: conversion state feeds
stream orientation and formatted I/O, while numeric and time functions reuse
existing narrow semantics. Treating every declaration, failure transition,
and buffer boundary as completion work prevents a header-only conformance
claim.

## Verification

**Method:** Automated compile-time and behavioral test, state-transition and
boundary analysis, regression, and personal review

**References:** `TC-0031`

TC-0031 shall exercise every named function through direct and `va_list`
entry points where applicable. It shall cover zero and one lengths, exact-fit
and short buffers, partial source consumption, invalid encodings, EOF/WEOF,
orientation conflicts, retained input, overlapping moves, token state,
numeric errors, and target ABI behavior. The full C89 suite and narrow C99
formatted-I/O suite shall run on x86 and x64; ARM64 shall compile/link locally
and run in retained native CI before target-complete acceptance.

## Relationships

- **Derived from:** C99 §7.24 and §7.8.2
- **Depends on:** REQ-0012, REQ-0013, REQ-0014, REQ-0015, REQ-0025,
  REQ-0027, REQ-0029, REQ-0030, REQ-0033, REQ-0039, and ADR-0003
- **Required by:** Any future Unicode GUI-startup work
- **Conflicts with:** Byte/wide I/O on one oriented stream, fixed-size
  conversion staging, or silent replacement of unrepresentable values

## Tailoring

Behavior is defined for WCRT's C locale and its documented single-byte
encoding. Locale-sensitive collation and transformation therefore use wide
code-unit lexical behavior for the representable repertoire. No `wWinMain`
entry point is part of this requirement.

For wide stream scanning, every code unit in the format must be representable
in the stream's captured C-locale encoding. An unrepresentable format code
unit is rejected with `EILSEQ` before input transfer. In-memory `swscanf`
format strings and wide string/memory functions retain arbitrary UTF-16 code
units. This is a documented WCRT encoding restriction; a future external
encoding or a format parser independent of that encoding requires a new
impact review.

## Implementation Record

`include/wchar.h` declares the complete C99 7.24 surface. The implementation
is divided into conversion, unformatted stream, string/numeric, time,
formatted-output, and formatted-input units. `include/inttypes.h` and
`src/wchar_string.c` also close `wcstoimax` and `wcstoumax`.

Restartable conversions use separate internal null-state objects and the
REQ-0033 single-byte state machine. Wide streams use the private orientation,
state, and pushback fields in `struct wcrt_file`. The wide formatter owns wide
sinks and exact wide `c`/`s` argument rules; the stream scanner reuses the
reviewed numeric and lookahead core in a wide-input mode, while `swscanf`
uses a UTF-16 code-unit parser so high code units are preserved in memory.
`wcsftime` uses the completed C99 C-locale `strftime` conversion set.

TC-0031 passes its presence, C89-isolation, behavior, format/scan, string,
conversion, stream, time, and numeric tests natively on x86, x64, and ARM64 in
exact-revision run `32027269426`. Full C89 and extension aggregates, Debug
library/consumer/startup builds, source-quality, traceability, and WSP-tool
gates also pass there. Exact local Release x86/x64 behavior, ARM64 compile/link,
and the Windows 2000 x86 import gate pass; native ARM64 Release and independent
review remain completion work.
