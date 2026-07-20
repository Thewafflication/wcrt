# REQ-0010 — `<stdarg.h>`

**Index:** [C89 Requirements](REQUIREMENTS.md)  
**Draft annotation:** §4.8 Variable arguments

## Required files

- `include/stdarg.h` — `va_list` and variable-argument macros.
- `src/internal/stdarg.h` — compiler/architecture capability layer if needed.
- `tests/c89/stdarg.c` — calling-convention and promoted-type tests.

## Public surface

- Type: `va_list`.
- §4.8.1.1: `va_start` macro.
- §4.8.1.2: `va_arg` macro.
- §4.8.1.3: `va_end` macro.

No linkable public function is required unless the compiler implementation uses
an internal helper.

## Requirements

- The macros shall implement each supported TinyCC calling convention and ABI.
- `va_start` shall initialize traversal using the last named parameter under
  the restrictions stated by the draft.
- `va_arg` shall honor default argument promotions, type size, and alignment.
- `va_end` shall complete traversal and permit the implementation to release or
  invalidate associated state.
- Multiple `va_list` traversal behavior shall not exceed C89 guarantees.
- Macro arguments shall not be evaluated more often than their contracts allow.

## Acceptance

Tests shall pass integers before and after promotion, floating arguments,
pointers, supported aggregate cases, alignment transitions, empty variadic
tails where valid, and multiple calls. Each supported architecture/calling
convention requires execution tests. Shared gates apply.
