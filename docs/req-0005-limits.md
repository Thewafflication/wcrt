# REQ-0005 — `<limits.h>`

**Index:** [C89 Requirements](REQUIREMENTS.md)  
**Draft annotation:** §4.1.4 Limits `<float.h>` and `<limits.h>`

## Required files

- `include/limits.h` — character and integer limits.
- `tests/c89/limits.c` — compile-time and representation checks.

No runtime source file or public function is required by this header.

## Required macros

`CHAR_BIT`, `SCHAR_MIN`, `SCHAR_MAX`, `UCHAR_MAX`, `CHAR_MIN`, `CHAR_MAX`,
`MB_LEN_MAX`, `SHRT_MIN`, `SHRT_MAX`, `USHRT_MAX`, `INT_MIN`, `INT_MAX`,
`UINT_MAX`, `LONG_MIN`, `LONG_MAX`, and `ULONG_MAX`.

## Requirements

- Values shall match the actual WCRT ABI and satisfy §2.2.4.2 minimum ranges.
- `CHAR_MIN` and `CHAR_MAX` shall reflect TinyCC's plain-`char` signedness for
  the selected target.
- Unsigned maxima and signed minima/maxima shall be usable as integer constant
  expressions with suitable types.
- `CHAR_BIT` shall agree with object representation and `sizeof` semantics.
- `MB_LEN_MAX` shall be no smaller than any supported locale's maximum
  multibyte character length.

## Acceptance

Tests shall verify macro presence, constant-expression usability, signedness,
range ordering, arithmetic relationships without overflowing test expressions,
and agreement with `sizeof`. The shared gates in `REQUIREMENTS.md` apply.

## Implementation record

- `include/limits.h` defines the Windows TinyCC ABI: 8-bit signed plain char,
  16-bit short, 32-bit int, and 32-bit long.
- `MB_LEN_MAX` is one because the currently supported `C` locale uses
  single-byte execution characters; REQ-0006 must update it before adding a
  wider multibyte encoding.
- `tests/c89/limits.c` verifies storage widths, signedness, unsigned wrap
  maxima, signed range relationships, and value round trips.
