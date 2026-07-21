# REQ-0011 — `<stddef.h>`

**Index:** [C89 Requirements](REQUIREMENTS.md)  
**Draft annotation:** §4.1.5 Common definitions

## Required files

- `include/stddef.h` — common types and macros.
- `tests/c89/stddef.c` — type, null pointer, and offset tests.

## Public surface

- Types: `ptrdiff_t`, `size_t`, `wchar_t`.
- Macros: `NULL`, `offsetof(type, member)`.

No public function is introduced by this header.

## Requirements

- Each type shall match the actual WCRT/TinyCC ABI and have the signedness and
  range required for its purpose.
- `NULL` shall be a valid null pointer constant in C89 expressions.
- `offsetof` shall yield a `size_t` constant expression giving the byte offset
  of the named member for valid complete structure types.
- Repeated inclusion and inclusion alongside all other standard headers shall
  not create conflicting typedefs or macro definitions.
- Any compiler builtin used by `offsetof` shall be capability-tested.

## Acceptance

Tests shall cover type sizes/signedness, pointer subtraction, `sizeof` result
assignment, wide-character storage, `NULL` conversions, nested structures,
arrays, padding, and `offsetof` constant-expression use. Shared gates apply.
