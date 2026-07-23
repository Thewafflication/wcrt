# REQ-0013 — `<stdlib.h>`

**Index:** [C89 Requirements](REQUIREMENTS.md)  
**Draft annotation:** §4.10 General utilities

## Required files

- `include/stdlib.h` — public types, macros, and declarations.
- `src/stdlib.c` or `src/stdlib/` — conversion and utility families.
- `src/platform/windows/heap.c` — allocation backend.
- `src/platform/windows/process.c` — environment, command, and termination.
- `src/internal/mbstate.h` — C89 multibyte conversion state.
- `tests/c89/stdlib.c` plus focused allocation/conversion/process tests.

## Functions

| Draft clause | Required functions |
| --- | --- |
| §4.10.1 String conversion | `atof`, `atoi`, `atol`, `strtod`, `strtol`, `strtoul` |
| §4.10.2 Pseudo-random sequence | `rand`, `srand` |
| §4.10.3 Memory management | `calloc`, `free`, `malloc`, `realloc` |
| §4.10.4 Environment communication | `abort`, `atexit`, `exit`, `getenv`, `system` |
| §4.10.5 Search and sort | `bsearch`, `qsort` |
| §4.10.6 Integer arithmetic | `abs`, `labs`, `div`, `ldiv` |
| §4.10.7 Multibyte character | `mblen`, `mbtowc`, `wctomb` |
| §4.10.8 Multibyte string | `mbstowcs`, `wcstombs` |

## Other public surface

Types `div_t`, `ldiv_t`, `size_t`, and `wchar_t`; macros `NULL`,
`EXIT_FAILURE`, `EXIT_SUCCESS`, `RAND_MAX`, and `MB_CUR_MAX`.

## Requirements

- Numeric conversion shall implement subject sequence, whitespace/sign/base,
  end pointer, overflow, return value, and `errno` requirements.
- Allocation shall provide alignment suitable for every C89 object type,
  preserve data correctly through `realloc`, and document permitted zero-size
  behavior.
- `atexit` shall support at least the required minimum and execute functions in
  reverse registration order during normal termination.
- `abort`, `exit`, `getenv`, and `system` shall use WCRT startup/environment
  state and Windows services rather than a host CRT.
- Search/sort comparator calls and element movement shall meet all size and
  ordering rules, including zero elements.
- Multibyte facilities shall support at least the `C` locale and share state
  only as the draft permits.

## Acceptance

Tests shall cover each function, conversion boundaries, allocation failure and
alignment, termination in child processes, environment lookup, sort/search
edge cases, integer overflow boundaries allowed by the contract, repeatable
random sequences, and multibyte state. Shared gates apply.

## C89 milestone design

Allocation uses the Windows process heap and treats zero-size requests as a
one-byte allocation that remains safe to free. Normal termination supports 32
reverse-order callbacks. Environment access and command execution use
`GetEnvironmentVariableA` and `CreateProcessA`; termination uses `ExitProcess`.
These APIs are available on Windows 2000. The mandatory C locale treats each
non-null byte as one multibyte character. Sorting uses an in-place stable
insertion algorithm; its complexity is secondary to the C89 contract.
