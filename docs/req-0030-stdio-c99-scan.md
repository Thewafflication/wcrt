# REQ-0030 — C99 narrow formatted input

**Content type:** Project requirement

**Status:** Implemented; exact native Debug x86/x64/ARM64 verification passes

**Source:** ISO/IEC 9899:1999 as corrected through TC3, §7.19.6.2,
§7.19.6.4, §7.19.6.7, §7.19.6.9, §7.19.6.11, and §7.19.6.14

## Scope

This requirement extends the REQ-0012 byte-oriented formatted-input family
with C99 modifiers, conversions, and the three `v*scanf` functions. Microsoft
`sscanf_s` remains an independently controlled compatibility interface. Wide
formatted-input functions and non-C locales are excluded.

### Required files

- `include/stdio.h` — C99 declarations and C89 isolation.
- `src/stdio_scan.c` — source handling, matching, conversions, and wrappers.
- `tests/c99/stdio-scan.c` — behavioral and boundary verification.

## Requirement

- In the C99 surface, WCRT shall declare `vfscanf`, `vscanf`, and `vsscanf`
  with the C99 `restrict` contracts. The selected C89 surface shall expose none
  of those declarations.
- WCRT shall apply `hh`, `ll`, `j`, `z`, and `t` to every applicable `d`, `i`,
  `o`, `u`, `x`, `X`, and `n` conversion using the exact destination pointer
  type required on x86, x64, and ARM64.
- WCRT shall accept `a`, `A`, `e`, `E`, `f`, `F`, `g`, and `G` floating input
  with the subject-sequence rules of REQ-0039. No modifier shall select
  `float *`, `l` shall select `double *`, and `L` shall select `long double *`.
- In the C locale, WCRT shall implement `lc`, `ls`, and `l[` using the 16-bit
  Windows wide-character ABI recorded by ADR-0002, including width and
  termination rules and encoding failure.
- Each input directive shall apply its field width while identifying the input
  item. The first character after the input item shall remain unread, and
  repeated stream calls shall observe that retained input.
- WCRT shall distinguish input failure from matching failure, return `EOF`
  only when an input failure occurs before the first assignment, and count
  assigned input items without counting suppressed assignments or `%n`.
- `vfscanf`, `vscanf`, and `vsscanf` shall consume their supplied `va_list`
  without invoking `va_end`; the value of the supplied list after return shall
  be indeterminate. Variadic wrappers shall initialize and end only their own
  lists.
- Formatted input shall not use a fixed line-staging limit or discard input
  beyond the characters required by the current conversion.

## Rationale

Correct destination types are an ABI requirement, not merely a width check.
Stream lookahead and failure classification are required for callers to recover
from matching failures and to perform repeated conversions reliably.

## Verification

**Method:** Automated compile-time and behavioral test, analysis, and review

**References:** `TC-0030`

TC-0030 shall verify declarations and C89 absence, every applicable modifier
and destination type, all floating spellings, matching and input failures,
assignment counts, suppression, `%n`, whitespace, widths, scansets, malformed
subjects, first-unread bytes, repeated stream calls, standard input, direct and
`va_list` entry points, and supported-target ABI behavior.

## Relationships

- **Derived from:** C99 formatted-input clauses listed in **Source**
- **Depends on:** REQ-0012, REQ-0025, REQ-0027, REQ-0039, and ADR-0002
- **Conflicts with:** A line-staging scanner that consumes unmatched input

## Tailoring

The `l` character forms are limited to WCRT's supported C locale. Results that
cannot be represented in a numeric destination are outside the C99-defined
behavior and are not assigned a WCRT extension by this requirement.

## Implementation Record

`src/stdio_scan.c` implements a shared string/stream input abstraction with
one-character lookahead, exact consumed counts, width-aware numeric lexical
state, exact destination-pointer selection, C-locale `lc`/`ls`/`l[`, and
distinct matching and input-failure results. Numeric input items use checked
growable storage rather than a fixed line record; only the first unread stream
byte is read ahead and it is restored before return.

`vfscanf`, `vscanf`, and `vsscanf` are the primary `va_list` entry points and
the variadic functions are wrappers. TC-0030 and the complete C89/extension
aggregates pass natively on x86, x64, and ARM64 in exact-revision run
`32027269426`.

## T3 Wide-character Implementation Impact

T3 routes `lc`, `ls`, and `l[` through the accepted REQ-0033 C-locale rules.
The scanner's shared input abstraction now has an internal wide-input mode
that preserves orientation and lookahead while selecting the inverse wide
`c`/`s` destination rules. The destination, width, termination, encoding, and
first-unread matrices pass on native x86/x64/ARM64 in exact-revision run
`32027269426`. Wide stream entry points remain owned by REQ-0031 rather than
this requirement.
