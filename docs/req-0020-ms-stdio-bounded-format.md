# REQ-0020 — Microsoft legacy bounded formatted output

**Content type:** Project requirement

**Status:** Implemented

**Source:** Microsoft Learn documentation for
[`_snprintf`](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/snprintf-snprintf-snprintf-l-snwprintf-snwprintf-l?view=msvc-170)
and
[`_vsnprintf`](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/vsnprintf-vsnprintf-vsnprintf-l-vsnwprintf-vsnwprintf-l?view=msvc-170)

## Scope

This requirement provides the narrow-character `_snprintf` and `_vsnprintf`
interfaces used by software written for the Microsoft CRT. Locale-specific,
wide-character, secure-template, and `_s` variants are excluded.

### Required files

- `include/stdio.h` — public compatibility declarations.
- `src/stdio_format.c` — legacy bounded formatted-output behavior.
- `tests/mscompat/presence/stdio.c` — declaration-presence verification.
- `tests/mscompat/stdio.c` — behavioral and boundary verification.

## Requirement

- WCRT's Microsoft compatibility surface shall declare `_snprintf` as `int
  _snprintf(char *buffer, size_t count, const char *format, ...);` and
  `_vsnprintf` as `int _vsnprintf(char *buffer, size_t count, const char
  *format, va_list arguments);`.
- Each function shall apply the same supported conversions as its WCRT
  standard counterpart.
- When the formatted length is less than `count`, each function shall store the
  complete output, append a null character, and return the formatted length.
- When the formatted length equals `count`, each function shall store exactly
  `count` characters, append no null character, and return `count`.
- When the formatted length exceeds `count`, each function shall store exactly
  `count` characters, append no null character, and return a negative value.
- When `buffer` is null and `count` is zero, each function shall write nothing
  and return the formatted length excluding the terminating null character.
- When `buffer` is nonnull and `count` is zero, each function shall leave the
  destination unchanged and return a negative value.
- `_vsnprintf` shall obtain its variable arguments from its `va_list`
  argument.
- These compatibility functions shall not change the C99 behavior required of
  `snprintf` and `vsnprintf` by REQ-0019.

## Rationale

Some Windows-oriented applications use the pre-C99 Microsoft names and depend
on their distinct truncation contract. A separate requirement makes that
behavior available without weakening WCRT's ISO C99 interfaces.

## Verification

**Method:** Automated compile-time and behavioral test

**References:** `TC-0020`

TC-0020 shall verify declarations, complete output, exact-limit output,
truncation, one-byte and zero-byte limits, null termination or its deliberate
absence, null-buffer length queries, and variadic forwarding.

## Relationships

- **Derived from:** Microsoft Learn `_snprintf` and `_vsnprintf`
  compatibility behavior linked in **Source**
- **Depends on:** REQ-0010 variable arguments and REQ-0019 bounded formatting
- **Conflicts with:** Applying the C99 truncation contract to the underscored
  compatibility names

## Tailoring

This requirement applies only when WCRT's Microsoft compatibility surface is
selected. These functions are not part of an ISO C conformance claim.

## Implementation Record

`include/stdio.h` exposes both compatibility declarations independently of the
selected ISO language mode. `src/stdio_format.c` gives the compatibility path
all `count` bytes, terminates only when the result is shorter than `count`, and
maps over-limit truncation to a negative return. TC-0020 passes its presence
and behavioral checks.
