# REQ-0019 — C99 bounded formatted output

**Content type:** Project requirement

**Status:** Implemented

**Source:** ISO/IEC 9899:1999 §7.19.6.5 and §7.19.6.12

## Scope

This requirement extends the C89 `<stdio.h>` baseline in REQ-0012 with the
C99 `snprintf` and `vsnprintf` functions. Other C99 formatted-output changes,
including new conversion specifiers and length modifiers, are excluded and
shall be specified separately.

### Required files

- `include/stdio.h` — public declarations.
- `src/stdio_format.c` — bounded formatted-output behavior.
- `tests/c99/presence/stdio.c` — declaration-presence verification.
- `tests/c99/stdio.c` — behavioral and boundary verification.

## Requirement

- WCRT's C99 `<stdio.h>` shall declare `snprintf` as `int snprintf(char *
  restrict s, size_t n, const char * restrict format, ...);` and `vsnprintf`
  as `int vsnprintf(char * restrict s, size_t n, const char * restrict format,
  va_list arg);`.
- Each function shall format output according to the formatted-output rules
  applicable to `sprintf` and `vsprintf`, respectively.
- When the destination size is greater than zero, each function shall write at
  most one less than the destination size in formatted characters and shall
  terminate the destination with a null character.
- When the destination size is zero, each function shall write no characters;
  the destination pointer may be null.
- On success, each function shall return the number of formatted characters
  that would have been written if the destination size were sufficiently
  large, excluding the terminating null character.
- Each function shall return a negative value when an encoding error occurs.
- `vsnprintf` shall obtain its variable arguments from its `va_list` argument;
  after the call, the value of the original argument shall be indeterminate.
- Adding these declarations and definitions shall not change the behavior or
  availability of the C89 formatted-output functions required by REQ-0012.

## Rationale

KerTeX and other bounded-formatting consumers require the standard C99
interfaces. Specifying truncation, null termination, zero-sized destinations,
and the would-have-written return value prevents substitution of incompatible
legacy `_snprintf` behavior.

## Verification

**Method:** Automated compile-time and behavioral test

**References:** `TC-0019`

TC-0019 shall verify independent declarations, ordinary formatting, exact-fit
and truncated destinations, one-byte and zero-byte destinations, a null
destination with zero size, return values, null termination, variadic
forwarding, error returns, and regression coverage for the REQ-0012 formatting
functions on every supported architecture.

## Relationships

- **Derived from:** C99 formatted input/output and the Milestone 2 roadmap
- **Depends on:** REQ-0010 variable arguments and REQ-0012 input/output
- **Conflicts with:** Legacy `_snprintf` semantics that do not guarantee C99
  null termination and would-have-written return values

## Tailoring

This requirement applies only when WCRT's C99 library surface is selected. It
does not add either function to WCRT's C89 conformance claim.

## Implementation Record

`include/stdio.h` exposes the standard declarations in C99 and later modes.
`src/stdio_format.c` uses the shared formatting engine with a bounded output
limit, reserves one destination byte for termination, and retains the logical
untruncated count. TC-0019 passes its presence and behavioral checks.
