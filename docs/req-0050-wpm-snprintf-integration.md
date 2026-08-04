# REQ-0050 — WPM access to bounded formatted output

**Content type:** Project requirement

**Status:** Proposed; interface decision required

**Source:** WPM TinyCC compatibility inventory,
[REQ-0019](req-0019-stdio-bounded-format.md), and Microsoft Learn
documentation for
[`snprintf`](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/snprintf-snprintf-snprintf-l-snwprintf-snwprintf-l?view=msvc-170)

## Scope

This requirement removes WPM's need to carry a reduced bounded formatter when
it is linked with WCRT. It covers WPM's current `wpm_snprintf` bridge and
WCRT's existing standard `snprintf`; it does not authorize unrelated WPM-owned
symbols in WCRT.

## Requirement

- A supported TinyCC/WCRT build of WPM shall be able to call WCRT's complete
  REQ-0019 bounded formatted-output implementation without compiling WPM's
  reduced formatter.
- The selected interface shall retain the C99 `snprintf` return, truncation,
  null-termination, and length-query contract.
- If supported WPM translation units compile in a language mode that does not
  expose WCRT's `snprintf` declaration, WCRT and WPM shall record whether to
  expose the declaration through compatibility-mode selection or to provide a
  temporary alias. The decision shall preserve C89 header conformance.
- WCRT shall not add `wpm_snprintf` to its stable public ABI unless an approved
  compatibility decision establishes ownership, lifetime, and deprecation.
- Verification shall prove that WPM resolves the selected symbol from WCRT and
  does not link a duplicate local implementation.

## Rationale

WPM renames its local fallback to `wpm_snprintf` when WCRT is selected. The
underlying capability already exists in WCRT under REQ-0019, so this is chiefly
a header-selection and integration issue rather than a new formatting family.

## Verification

**Method:** Build integration, symbol inspection, and behavioral test

**References:** Planned `TC-0050`

Tests shall build the affected WPM configuration, inspect the resolved symbol,
exercise WPM's used conversion set and truncation paths, and rerun TC-0019.

## Relationships

- **Derived from:** WPM TinyCC compatibility inventory
- **Depends on:** REQ-0019 standard bounded formatting
- **Conflicts with:** Permanently adopting a downstream namespace without an
  explicit ABI decision

## Tailoring

The final interface is intentionally unresolved until the TinyCC language-mode
and header-selection behavior is reproduced in the WPM integration build.

## Implementation Record

WCRT implements `snprintf` under REQ-0019. WPM currently compiles a limited
formatter as `wpm_snprintf` from `wpm/tcc_compat/secure_crt.c` when WCRT is
selected.
