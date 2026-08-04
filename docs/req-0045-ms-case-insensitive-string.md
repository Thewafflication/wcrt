# REQ-0045 — Microsoft case-insensitive narrow-string comparison

**Content type:** Project requirement

**Status:** Implemented

**Source:** Microsoft Learn documentation for
[`_stricmp`](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/stricmp-wcsicmp-mbsicmp-stricmp-l-wcsicmp-l-mbsicmp-l?view=msvc-170)
and
[`_strnicmp`](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/strnicmp-wcsnicmp-mbsnicmp-strnicmp-l-wcsnicmp-l-mbsnicmp-l?view=msvc-170);
WPM TinyCC compatibility inventory

**Compatibility annotation:** Microsoft CRT extension; not ISO C

## Scope

This requirement provides the narrow-character `_stricmp` and `_strnicmp`
interfaces needed by WPM. Wide, multibyte, explicit-locale, collation, and
deprecated non-underscored aliases are excluded.

## Requirement

- WCRT shall declare `_stricmp(const char *, const char *)` and
  `_strnicmp(const char *, const char *, size_t)` in `<string.h>`.
- Each function shall compare characters after lowercase conversion under the
  active `LC_CTYPE` category and return a value less than, equal to, or greater
  than zero according to the first differing converted characters.
- `_strnicmp` shall compare no more than `count` characters and shall return
  zero without accessing either string when `count` is zero.
- Comparison shall stop at the first null character.
- Invalid null string arguments shall invoke WCRT's Microsoft
  invalid-parameter policy; if execution continues, the functions shall return
  `_NLSCMPERROR` and set `errno` to `EINVAL`.
- WCRT shall not expose `stricmp`, `strcmpi`, or `strnicmp` aliases under this
  requirement.

## Rationale

WPM currently carries these functions because its TinyCC/WCRT build cannot
obtain them from WCRT.

## Verification

**Method:** Automated compile-time and behavioral test

**References:** Planned `TC-0045`

Tests shall cover case-only differences, ordering, punctuation, empty strings,
high-bit bytes, count boundaries, termination, locale behavior supported by
WCRT, invalid parameters, and absence of excluded aliases.

## Relationships

- **Derived from:** WPM TinyCC compatibility inventory
- **Depends on:** REQ-0006 locale and REQ-0014 string handling
- **Conflicts with:** Treating these names as ISO C interfaces

## Tailoring

This requirement applies only to the Microsoft compatibility surface.

## Implementation Record

`include/string.h` declares both functions and `src/string.c` implements the
Microsoft comparison and invalid-parameter continuation behavior. TC-0045
passes on x86 and x64 TinyCC.
