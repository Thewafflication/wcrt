# ADR-0005 — C99 complex representation and compiler-blocked release profile

**Status:** Accepted; temporary compiler block resolved by TinyCC 1441 and
requalified for the T6-selected TinyCC 1442 package

**Date:** 2026-08-12

## Context

At the decision date, WCRT's supported TinyCC package could not complete the
required complex language model. Packaged TinyCC
`0.9.28-rc.1437+2be0218b` reports `_Complex is not yet
supported` through its bundled i386, x86_64, and AArch64 drivers. Additional
target compiler binaries expose other front-end paths: a `_Complex` object
declaration reports `'{` expected and an `fi` literal reports `invalid number`.
`_Generic` is available as an extension. T5
implementation and review can proceed independently, and releases must
continue to build, test earlier requirements, and produce WPM packages while
the compiler issue is resolved.

TinyCC `0.9.28-rc.1441+0af32d51` subsequently passed both complex probes on
2026-08-13 for i386, x86_64, and AArch64 Windows. The automatic supported path
in this decision is therefore active; the ExpectedFail rules remain only as a
controlled regression fallback.

T6 selected `0.9.28-rc.1442+2474e1c2` (source revision
`d5c02f0fcdfdf75265d38df6ff9db2f8067367ac`) and re-ran the same language
gates. The selected package remains on the Supported path. This does not
transfer T5's historical native ARM64 result to the T6 candidate baseline.

On 2026-08-21 the latest-package policy selected TinyCC
`0.9.28-rc.1444+9a4be30f`. Upstream revision `9a4be30f` corrected its complex
runtime helper declaration to a standard function returning through an output
pointer. On Windows ARM64 this places the four scalar arguments in `d0`--`d3`
and the pointer in `x0`. The earlier WCRT 1442 register remap therefore became
incorrect and was removed; the target assembly now supplies only symbol aliases
that preserve the corrected ABI. This is controlled by TC-0037 and the
static/DLL consumer gates, not inferred from the compiler change.

## Decision

- The public type remains compiler-owned `_Complex`; WCRT will not substitute
  a structure and claim C99 conformance.
- The implementation uses the C99 guarantee that each complex representation
  corresponds to two adjacent values of its real type, in real/imaginary order.
  Internal unions construct and inspect those components. ABI acceptance still
  requires compiler and cross-translation-unit probes on every target.
- Optional `_Imaginary` types and `#pragma STDC CX_LIMITED_RANGE` behavior are
  not claimed until the compiler supplies and passes their probes.
- `src/complex.c` is compiled into WCRT only when the selected compiler passes
  the complex capability gate. Until then the legacy C89 build deliberately
  omits it and the package records that the complex runtime is unavailable.
- TC-0037 and TC-0038 may be `ExpectedFail` only when both source-specific
  probes match ADR-0005's exact diagnostic matrix: the arithmetic/type probe
  may report `_Complex is not yet supported` or `'{` expected `(got ';')`, and
  the imaginary-constant probe may report `_Complex is not yet supported` or
  `invalid number`. ExpectedFail is non-fatal to CI and release packaging but
  is not Pass or conformance evidence. Every unrelated failure remains fatal.
- Once the capability probe succeeds, expected-failure classification is
  disabled automatically, `src/complex.c` becomes required in shared/static
  builds, and all native ABI, numeric, dispatch, consumer, and release tests are
  mandatory.
- WCRT remains a non-Annex-G implementation. Complex functions retain `errno`
  produced by composed T4 real targets but do not add independent error writes
  or claim automatic floating exceptions. T4's explicit ordinary tolerances
  remain the underlying real-target profile.

## Consequences

The source, headers, specifications, and tests can be reviewed and shipped
without hiding the compiler dependency. Packages built with TinyCC 1437 omit
complex symbols as recorded; packages built with a qualified supported package
include the runtime and complete capability evidence. T6 retains exact TinyCC
1442 evidence; successor workflows select the latest eligible WPM package and
record its exact identity. Release readiness still requires native behavior
evidence and review rather than inferring them from probe or cross-compilation
success.
