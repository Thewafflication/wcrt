# Byte-pair copying tranche

## Plan and design

Implement REQ-0097 _swab in stdlib.h/stdlib.c using the legacy char-pointer
signature. Copy adjacent pairs, retain an odd trailing destination byte,
support exact in-place operation by loading both bytes before writing, and
report EINVAL for null pointers or negative counts without touching memory.
Partial overlap remains outside the selected contract. Existing errno is
preserved on successful zero/one-byte no-op and pair copies.

Estimate 15-25 implementation lines, 70-110 fixture lines and 20-40 minutes.
Review byte values, signed count boundaries, termination/guard preservation,
and source/default-cdecl function pointer compatibility. Require direct and
package tests on x86/x64, ARM64 cross-links, supplementary Microsoft consumers,
source quality, traceability and import gates. This additive export requires
rebuilt packages; no dependency or Windows API is added. Genuine VC6/reference
membership and arbitrary overlap behavior remain Unknown.

The current [Microsoft _swab documentation](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/swab)
informs selected pair-copy and odd-count behavior. Its modern parameter-handler
mechanism is outside the WCRT contract; errno is the selected error channel.
Actual effort will not be fabricated.

## Implementation and review

The presence fixture first reproduced the missing _swab declaration. The
implementation validates before arithmetic/access, computes the pair loop
without count+1 overflow, and loads both bytes before writing. At INT_MAX the
last pair index increments only to INT_MAX-1, where the loop terminates.
No source or destination byte is read for zero/one counts; invalid pointers
are rejected before those no-op paths. No string termination is appended.
Fixtures check 258 distinct lengths, all byte values and embedded zero,
retained odd tails, disjoint source preservation and exact in-place results.
Function-pointer calls also execute under supplementary MSVC /Od and /O2.
Focused native/package x86/x64 tests and ARM64 cross-links passed. Source
quality passed 359 C/H files; traceability passed 96 mappings. Full native
regressions and retained evidence validation are running before closeout.

## Closeout

Both native targets passed all 15 C89 and 78 extension cases. Microsoft major
16 executed the eleven-unit consumer at /Od and /O2 without host CRT imports;
all 85 retained input hashes matched source and rebuilt runtime. Focused
static/DLL packages passed x86/x64 and ARM64 compiled/linked only. Source
quality passed 359 files with zero violations; traceability passed 96 mappings.
No new Windows imports were added. Implementation is 16 lines plus a declaration,
within estimate; actual effort was not reliably timed. VC6/reference and
arbitrary overlap qualification remain open.
