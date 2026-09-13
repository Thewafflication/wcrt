# REQ-0068 — Selected POSIX filename matching

**Content type:** Project requirement

**Status:** Implemented

**Source:** POSIX.1-2017 `fnmatch`

## Scope

This requirement provides selector-gated filename-pattern matching without a
host CRT or Shlwapi dependency. Locale collation classes are excluded.

## Requirement

- `<fnmatch.h>` shall expose `fnmatch`, `FNM_NOMATCH`, `FNM_PATHNAME`,
  `FNM_NOESCAPE`, and `FNM_PERIOD` only under `WCRT_POSIX`.
- `FNM_CASEFOLD` shall provide an ASCII case-insensitive extension.
- Matching shall support `*`, `?`, bracket sets, ranges, negation, and escapes.
- `FNM_PATHNAME` shall prevent wildcards from matching `/`; `FNM_PERIOD` shall
  require an explicit leading period at a pathname-component boundary.
- The implementation shall add no Windows imports.

## Verification

**Method:** Selected/strict C89/C99 compilation and behavioral partitions

**References:** TC-0068

## Relationships

- **Depends on:** REQ-0002 and REQ-0050
- **Conflicts with:** Delegating wildcard semantics to Shlwapi

## Implementation Record

`include/fnmatch.h` defines the selected surface and `src/fnmatch.c` implements
the matcher. TC-0068 covers literals, wildcards, path boundaries, periods,
brackets, escaping, case folding, mismatch, and header isolation.
