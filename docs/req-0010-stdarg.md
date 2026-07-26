# REQ-0010 — `<stdarg.h>`

**Content type:** Project requirement

**Status:** Implemented

**Source:** §4.8 Variable arguments

## Scope

### Required files

- `include/stdarg.h` — `va_list` and variable-argument macros.
- `src/internal/stdarg.h` — compiler/architecture capability layer if needed.
- `tests/c89/stdarg.c` — calling-convention and promoted-type tests.

### Public surface

- Type: `va_list`.
- §4.8.1.1: `va_start` macro.
- §4.8.1.2: `va_arg` macro.
- §4.8.1.3: `va_end` macro.

No linkable public function is required unless the compiler implementation uses
an internal helper.

## Requirement

- The macros shall implement each supported TinyCC calling convention and ABI.
- `va_start` shall initialize traversal using the last named parameter under
  the restrictions stated by the draft.
- `va_arg` shall honor default argument promotions, type size, and alignment.
- `va_end` shall complete traversal and permit the implementation to release or
  invalidate associated state.
- Multiple `va_list` traversal behavior shall not exceed C89 guarantees.
- Macro arguments shall not be evaluated more often than their contracts allow.

## Rationale

WCRT must provide its own `<stdarg.h>` contract to supply C89-conforming behavior without depending on a host C runtime.

## Verification

**Method:** Automated test and inspection

**References:** `TC-0010`

Tests shall pass integers before and after promotion, floating arguments,
pointers, supported aggregate cases, alignment transitions, empty variadic
tails where valid, and multiple calls. Each supported architecture/calling
convention requires execution tests. Shared gates apply.

## Relationships

- **Derived from:** §4.8 Variable arguments
- **Depends on:** Shared build, ABI, and quality gates in `docs/REQUIREMENTS.md`
- **Conflicts with:** None known

## Tailoring

WCRT groups the related obligations for this C89 conformance unit under one
stable requirement identifier. Each observable obligation remains explicit in
this record and is verified collectively by `TC-0010`.

## Implementation Record

WCRT exposes TinyCC's `__builtin_va_list`, `__builtin_va_start`,
`__builtin_va_arg`, and `__builtin_va_end` through the standard C89 names.
TinyCC therefore owns the architecture-specific register, stack, size, and
alignment traversal rules for each target ABI; WCRT adds no host CRT helper.
