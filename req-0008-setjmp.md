# REQ-0008 — `<setjmp.h>`

**Index:** [C89 Requirements](REQUIREMENTS.md)  
**Draft annotation:** §4.6 Non-local jumps

## Required files

- `include/setjmp.h` — `jmp_buf` and public interfaces.
- `src/setjmp.c` — portable runtime half where needed.
- `src/platform/x86/setjmp.asm` — x86 context operations if compiler support is
  insufficient.
- `tests/c89/setjmp.c` — control-flow, values, nesting, and register tests.

## Public surface

| Draft clause | Facility |
| --- | --- |
| §4.6.1.1 | `setjmp` (permitted to be a macro) |
| §4.6.2.1 | `longjmp` |

The header shall define `jmp_buf`.

## Requirements

- The saved environment shall match each supported WCRT ABI, including stack
  state, instruction location, and all nonvolatile registers.
- The direct invocation of `setjmp` shall return zero.
- `longjmp(env, value)` shall resume the matching `setjmp`, which returns
  `value`, except that a zero value shall be observed as 1.
- Automatic non-`volatile` objects modified after `setjmp` shall retain only the
  guarantees given by the draft.
- Signal-mask behavior, if any, shall be documented as an extension and shall
  not alter the C89 contract.
- Invalid lifetime, cross-thread, and unwound-context uses shall be documented
  as undefined and shall not be used by acceptance tests.

## Acceptance

Tests shall cover zero/nonzero values, nested calls, repeated jumps where valid,
callee-saved integer and floating state, stack restoration, and `volatile`
objects at optimization levels supported by TinyCC. Shared gates apply.
