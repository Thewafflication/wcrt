# REQ-0025 — C99 variable arguments and `restrict` contracts

**Content type:** Project requirement

**Status:** Approved; implementation planned

**Source:** ISO/IEC 9899:1999 §6.7.3 and §7.15

## Scope

This requirement adds `va_copy` and controls the use of C99 `restrict` in
public declarations inherited from the C89 library.

## Requirement

- In C99 and later modes, WCRT's `<stdarg.h>` shall define `va_copy(dest, src)`
  so that `dest` independently traverses the same argument sequence and
  position represented by `src` at the time of copying.
- Each successful `va_copy` invocation shall be matched by `va_end` on the
  destination before the destination is reinitialized or leaves scope.
- Traversing or ending either copy shall not change the traversal position of
  the other copy.
- C99 public function declarations shall include every `restrict` qualification
  required by their standard declarators.
- WCRT shall express those qualifications through an edition-aware header
  mechanism that expands to C99 `restrict` in C99 and later modes and exposes
  no `restrict` token in C89 mode.
- These declaration changes shall preserve the linkage and calling convention
  of the corresponding C89 functions.

## Rationale

Independent variable-argument traversal is needed by forwarding functions, and
edition-aware declarations preserve both C99 contracts and C89 parsability.

## Verification

**Method:** Automated compile-time, behavioral, and header inspection test

**References:** `TC-0025`

TC-0025 shall verify independent copies at the start and after partial
traversal, cleanup, nested forwarding, required `restrict` declarations, C89
parsing, and ABI compatibility on every supported target.

## Relationships

- **Derived from:** C99 Milestone 2A
- **Depends on:** REQ-0010 and the public-header edition selection policy
- **Conflicts with:** A simple assignment on targets where `va_list` is not
  safely assignable

## Tailoring

The `restrict` audit is limited to public declarations introduced or changed by
C99; it does not change runtime behavior beyond the standard pointer contracts.
