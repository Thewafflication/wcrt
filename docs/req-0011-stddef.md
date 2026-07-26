# REQ-0011 — `<stddef.h>`

**Content type:** Project requirement

**Status:** Implemented

**Source:** §4.1.5 Common definitions

## Scope

### Required files

- `include/stddef.h` — common types and macros.
- `tests/c89/stddef.c` — type, null pointer, and offset tests.

### Public surface

- Types: `ptrdiff_t`, `size_t`, `wchar_t`.
- Macros: `NULL`, `offsetof(type, member)`.

No public function is introduced by this header.

## Requirement

- Each type shall match the actual WCRT/TinyCC ABI and have the signedness and
  range required for its purpose.
- `NULL` shall be a valid null pointer constant in C89 expressions.
- `offsetof` shall yield a `size_t` constant expression giving the byte offset
  of the named member for valid complete structure types.
- Repeated inclusion and inclusion alongside all other standard headers shall
  not create conflicting typedefs or macro definitions.
- Any compiler builtin used by `offsetof` shall be capability-tested.

## Rationale

WCRT must provide its own `<stddef.h>` contract to supply C89-conforming behavior without depending on a host C runtime.

## Verification

**Method:** Automated test and inspection

**References:** `TC-0011`

Tests shall cover type sizes/signedness, pointer subtraction, `sizeof` result
assignment, wide-character storage, `NULL` conversions, nested structures,
arrays, padding, and `offsetof` constant-expression use. Shared gates apply.

## Relationships

- **Derived from:** §4.1.5 Common definitions
- **Depends on:** Shared build, ABI, and quality gates in `docs/REQUIREMENTS.md`
- **Conflicts with:** None known

## Tailoring

WCRT groups the related obligations for this C89 conformance unit under one
stable requirement identifier. Each observable obligation remains explicit in
this record and is verified collectively by `TC-0011`.

## Implementation Record

The public types use TinyCC's `__SIZE_TYPE__`, `__PTRDIFF_TYPE__`, and
`__WCHAR_TYPE__` ABI definitions. `NULL` is the integer constant zero, making
it usable for both object and function pointers, and `offsetof` uses
TinyCC's constant-expression `__builtin_offsetof` facility.
