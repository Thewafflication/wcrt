# VC6-era I64 formatted integer tranche

## Plan and estimate

Scope: REQ-0077 `%I64` integer output/input length modifier in the narrow
format families. Reuse the existing exact-width long-long argument/store
paths. No new symbols or compiler facilities. Estimate: 10-20 implementation
lines and 100-180 test lines, 30-60 minutes. Release effect: additive Microsoft
format extension; ISO directives and legacy bounded-output contracts unchanged.

## Design and review plan

Recognize the three-byte modifier after width/precision and before conversion;
map to the existing LL enum in both engines. Check truncated format strings,
vararg advancement on x86, signed extrema, unsigned radix formats, width,
precision, `%n` pointer width, suppression, match failure and stream lookahead.
Run existing TC-0029/0030 and TC-0020, new TC-0077 on x86/x64, ARM64 links,
package/static/DLL coverage and controlled source/traceability checks. Genuine
VC6 reference qualification remains Unknown without that environment.

## Closeout

On 2026-10-01 TC-0077 passed native x86/x64, including packaged static/DLL
explicit-startup consumers without host CRT imports. ARM64 sources and
packaged consumers compile/link; native execution remains Unknown. Targeted
TC-0029, TC-0030 and TC-0020 passed. Source changes total eight lines, within
estimate. Review confirmed short-circuit reads of incomplete modifiers and
reuse of existing typed LL argument/count paths. No production defect was
found in verification. Full extension regression is recorded with the header
foundation tranche. Genuine VC6 differential qualification remains Unknown.
