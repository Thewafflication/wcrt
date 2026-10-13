# Microsoft physical descriptor EOF tranche

## Plan, design and estimate

REQ-0089 adds _eof as a physical descriptor-position query using REQ-0088's
length and position interfaces. Estimate: 70-130 fixture/implementation lines,
30-60 minutes. The selected contract returns one only when current position
equals current size, including an empty file; positions beyond end return zero.
It neither reads nor seeks to end, and preserves all stream state. Length and
position are independent snapshots, so concurrent external changes require
caller coordination. Nonregular/invalid descriptors and provider errors reuse
the existing controlled vocabulary. No FILE-EOF-state or DOS Ctrl-Z profile
is inferred, and genuine VC6 reference qualification remains Unknown.

Review type signature, exact-end/beyond-end partitions, text physical size,
EOF flag independence, pushback retention, duplicate positions, invalid/pipe
descriptors and provider failures. Complete with native/package x86/x64,
ARM64 cross-links, Microsoft consumer, controlled header inventory, regression,
quality, traceability and defect/effort records. No new import, binary-layout
or dependency change is planned.

## Review and verification

On 2026-10-01 native/static/DLL fixtures passed x86 and x64; ARM64 fixtures
and packages cross-linked without execution. Deterministic queries verify
signed-64-bit maximum equality, early return on metadata failure and retained
position-query errors. The Microsoft major-16 selected-header DLL consumer
passed both /Od and /O2 at exact and beyond-end positions without CRT imports.
TC-0016 passed 327 C/H files and traceability passed 88 requirements/specs/
implementations. Windows 2000 x86 import audit passed. Full 70-case aggregates
are retained with the next checkpoint; genuine VC6 qualification stays Unknown.

Personal review isolated the small query in src/ms_eof.c so failure injection
can replace its dependencies without duplicating the full stdio implementation.
The initial mock had omitted its query-under-test; this test-construction
defect was caught by source review before execution and corrected by including
the isolated implementation. An 81-character documentation line was shortened
after source-quality verification. About 125 fixture/implementation lines were
added. Effort was not reliably timed; the shared query implementation kept
filesystem mutation and extra Windows imports out of this tranche.

The completed 70-case extension aggregates subsequently passed on x86 and
x64, retaining both descriptor-size and physical-EOF cases in the full suite.
