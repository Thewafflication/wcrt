# C89 Microsoft consumer call/return ABI coverage

## Plan and review design

Extend REQ-0079's real consumer beyond declaration/varargs checks to C89
small-structure returns, floating-point argument/return/out-parameters,
conversion end pointers and comparator callbacks. Estimate: 80-140 fixture
lines and 30-90 minutes. Dependencies: existing C89 functions and Microsoft
consumer runner. Risks: x86 aggregate-return differences, optimizer intrinsics
hiding calls and compiler helper dependencies. Volatile function pointers
force real calls where intrinsic substitution would invalidate evidence.

Inspect a minimal small-return object/assembly before executing the probe,
then compare div/ldiv results, exact binary floating cases, parse stopping
pointers, qsort/bsearch callbacks and allocation results. No public ABI is
changed to conceal a mismatch; record any incompatibility as a material finding.
Run TinyCC controls, Microsoft /Od-/O2 DLL consumers, quality and regression
gates. Genuine VC6 compiler/reference qualification stays Unknown. Timing is
not reliably measured. No new runtime function or Windows import is planned.

## Review and verification

On 2026-10-01 TinyCC x86/x64 controls and Microsoft major-16 /Od-/O2 DLL
consumers passed div/ldiv structure returns, floating calls/out-parameters,
parse end pointers, comparator callbacks and allocation/reallocation/zeroing.
Small-return inspection showed both compilers return the two words in EAX/EDX.
Microsoft dumpbin could not decode the TinyCC ELF object and lacked its
disassembler DLL; direct text-section bytes and Microsoft /FA assembly supplied
the inspection, followed by actual runtime evidence. No calling ABI changed.

The first fixture assumed sqrt domain failure returns NaN, contrary to WCRT's
controlled C89 contract (zero with EDOM). The TinyCC control failed at code 4;
review of REQ-0007 corrected that test assumption before Microsoft execution.
This is a test-construction defect, not a relaxed product requirement. Volatile
function pointers keep div/ldiv/sqrt/pow calls from being hidden by intrinsics.
About 90 fixture lines were added; effort was not reliably timed. Genuine
VC6 compiler/reference evidence remains Unknown. Final quality/cross-link gates
and retained consumer input/tool fingerprints follow in the next checkpoint.

The retained Microsoft record now identifies compiler 16.00.30319.01, three
tool entry-point hashes, 23 standalone headers, eight consumer translation
units and 81 input hashes including headers, sources, scripts, generated
standalone units, DLL and export definition. /Od and /O2 each carry executable
hashes, zero exit status and no-host-CRT evidence. All input/tool hashes are
checked for mutation before publishing the record. A TinyCC-only rerun was
verified to preserve the optional Microsoft record. Input/executable hashes
were independently rechecked after publication. Package C89 call controls
passed x86/x64; ARM64 cross-linked only. TC-0016 later passed 339 files and
traceability passed 91 mappings. This improves retained supplementary evidence
without assigning genuine VC6 or reference-runtime qualification.
