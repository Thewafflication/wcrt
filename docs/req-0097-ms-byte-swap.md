# REQ-0097: Microsoft byte-pair copying

stdlib.h shall declare default-cdecl void _swab(char *source, char *destination,
int count). For valid buffers and nonnegative counts it copies/swaps adjacent
byte pairs through count rounded down to even. An odd trailing byte is not
copied and bytes outside the complete-pair range remain unchanged. Support
disjoint buffers and exact in-place operation; partial overlap is unspecified.
Do not interpret high-bit bytes as characters or stop at embedded zero.
Success preserves errno; zero and one counts access no buffer bytes.

Null source/destination or negative count shall set EINVAL and leave all
accessible memory untouched, including INT_MIN; no parameter handler is added.
The return type is void and source pointer matches the legacy mutable spelling.
No allocation, host CRT or Windows API dependency is introduced.

## Verification and impact

TC-0097 exercises function-pointer signatures, seeded guards/destination bytes,
even/odd/zero/one and negative counts, all byte values, embedded zeros, exact
in-place copies and independent source preservation. Native and static/DLL
package x86/x64, ARM64 cross-links and supplementary Microsoft calls apply.
Update controlled runner/manifest/inventory and source quality/traceability;
rebuild packages for the additive export. Work record covers design/review.
Genuine VC6 and reference-runtime qualification remain Unknown.
