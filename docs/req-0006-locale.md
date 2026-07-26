# REQ-0006 — `<locale.h>`

**Content type:** Project requirement

**Status:** Implemented

**Source:** §4.4 Localization

## Scope

### Required files

- `include/locale.h` — locale types, categories, and declarations.
- `src/locale.c` — locale selection and convention data.
- `src/internal/locale.h` — private immutable/mutable locale representation.
- `tests/c89/locale.c` — category, query, formatting, and failure tests.

### Public surface

| Draft clause | Functions |
| --- | --- |
| §4.4.1.1 | `setlocale` |
| §4.4.2.1 | `localeconv` |

The header shall define `struct lconv`, `NULL`, `LC_ALL`, `LC_COLLATE`,
`LC_CTYPE`, `LC_MONETARY`, `LC_NUMERIC`, and `LC_TIME`.

## Requirement

- The `"C"` locale shall always be supported and shall be the startup locale.
- `setlocale(NULL)` queries shall not modify state.
- Category changes shall affect only their selected categories; `LC_ALL` shall
  query or update the complete locale as specified.
- An unsupported locale request shall return `NULL` without silently selecting
  another locale.
- Returned locale strings and `struct lconv` storage shall obey the draft's
  lifetime and overwrite rules.
- `localeconv` fields shall match formatted I/O, numeric conversion, collation,
  and monetary conventions implemented by WCRT.
- Additional Windows locale mapping is optional for the C89 milestone and shall
  not compromise the mandatory `C` locale.

## Rationale

WCRT must provide its own `<locale.h>` contract to supply C89-conforming behavior without depending on a host C runtime.

## Verification

**Method:** Automated test and inspection

**References:** `TC-0006`

Tests shall cover startup state, query/update/restore, each category, invalid
names, `struct lconv` fields, and interactions with `ctype`, `strcoll`,
`strxfrm`, numeric conversion, and `strftime`. Shared gates apply.

## Relationships

- **Derived from:** §4.4 Localization
- **Depends on:** Shared build, ABI, and quality gates in `docs/REQUIREMENTS.md`
- **Conflicts with:** None known

## Tailoring

WCRT groups the related obligations for this C89 conformance unit under one
stable requirement identifier. Each observable obligation remains explicit in
this record and is verified collectively by `TC-0006`.

## Implementation Record

- `include/locale.h` declares all C89 categories, `struct lconv`, `setlocale`,
  and `localeconv`.
- `src/locale.c` implements the mandatory startup `C` locale. The empty locale
  name also selects `C`; unsupported names fail without changing state.
- `src/internal/locale.h` contains private category and locale-name validation
  contracts.
- `tests/c89/locale.c` covers startup, every category, queries, invalid
  requests, every convention member, and the implemented ctype interaction.
- Collation, numeric conversion, and time-format integration checks execute
  with REQ-0014, REQ-0013, and REQ-0015 respectively.
