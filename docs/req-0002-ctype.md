# REQ-0002 — `<ctype.h>`

**Index:** [C89 Requirements](REQUIREMENTS.md)  
**Draft annotation:** §4.3 Character handling

## Required files

- `include/ctype.h` — public declarations or permitted macro forms.
- `src/ctype.c` — classification tables/logic and case mapping.
- `tests/c89/ctype.c` — full-domain and locale tests.

## Functions

| Draft clause | Required functions |
| --- | --- |
| §4.3.1 Character testing | `isalnum`, `isalpha`, `iscntrl`, `isdigit`, `isgraph`, `islower`, `isprint`, `ispunct`, `isspace`, `isupper`, `isxdigit` |
| §4.3.2 Case mapping | `tolower`, `toupper` |

## Requirements

- Every function shall accept `EOF` or any value representable as `unsigned
  char`; other argument values retain the draft's undefined behavior.
- Classification results shall be zero or nonzero as specified and shall obey
  all required class relationships.
- The `C` locale shall classify the required execution character set exactly as
  specified by §4.3.1.
- `tolower` and `toupper` shall return the mapped value when a mapping exists
  and the input value unchanged otherwise.
- Results shall follow the active `LC_CTYPE` locale.
- Macro forms may evaluate their arguments only once and addressable function
  forms shall be supplied where §4.1.6 requires them.

## Acceptance

Tests shall cover `EOF`, every value from 0 through `UCHAR_MAX`, class
relationships, case round trips, argument side effects for macros, and all
supported locales. The shared gates in `REQUIREMENTS.md` apply.
