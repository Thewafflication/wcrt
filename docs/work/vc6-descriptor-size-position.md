# Microsoft descriptor size and 64-bit position tranche

## Plan and estimate

REQ-0088 adds _lseeki64/_telli64 and _filelength/_filelengthi64, addressing
missing VC6-era low-I/O spellings using existing descriptor and status backends.
Estimate: 120-230 implementation/test lines, 45-100 minutes. Dependencies:
REQ-0067 corrected seek backend, REQ-0049 wide status and REQ-0078 Microsoft
integer spelling. Risks: size narrowing, stream-state changes on queries,
nonseekable handles and Microsoft call/return ABI. Complete with native x86/x64,
package/static/DLL and ARM64 cross-links, deterministic boundary/error tests,
Microsoft consumer probes, io.h inventory, regression and quality records.

## Design and review plan

_lseeki64 uses the existing seek path and resets stream EOF/pushback/conversion
state after successful repositioning. _telli64 queries the raw backend at zero
SEEK_CUR without clearing stream state. Length queries reuse _fstat64, avoid
moving the shared file position and accept regular files only; nonregular
handles report ESPIPE. Narrow _filelength reports EFBIG above LONG_MAX, wide
length retains every nonnegative signed 64-bit result. Negative provider sizes
report EIO. Descriptor/provider failures keep existing error mappings.

Review signed extrema, invalid origins/descriptors, sparse positions without
large-file creation, duplicate positions, text physical size, EOF/pushback
preservation, and type-compatible function pointers. These are controlled WCRT
contracts; genuine VC6 membership and differential qualification remain Unknown.
No new Windows imports or existing record/stream ABI changes are planned.

## Verification and review findings

On 2026-10-01 native, static/DLL packaged and deterministic provider fixtures
passed x86 and x64. ARM64 direct and package cross-links passed, without native
execution. Microsoft major-16 selected-header DLL consumer passed /Od and /O2,
including wide argument/return values and positions above 4 GiB. TC-0043 and
TC-0067 regressions passed both native targets. Queries compare complete
stream snapshots, including pending pushback/error/EOF state; repositioning
checks that pending pushback is discarded. Read-only and write-only file
descriptors are covered without changing contents during queries.

Personal review corrected a duplicate aggregate entry and the fixture's
initial reliance on opaque FILE fields by explicitly including its internal
test-only definition. Source quality found an 81-character documentation line,
shortened without behavior change; final quality/C89 gates follow. Baseline
missing interface coverage is corrected; no backend behavior was relaxed.
About 210 implementation/test lines were added, within the estimated range.
Effort was not reliably timed. Genuine VC6 qualification remains Unknown.

The full 15-case C89 suites subsequently passed on x86 and x64. The later
EOF tranche retained source-quality Pass for 327 files, traceability Pass for
88 requirement/specification/implementation mappings and Windows 2000 x86
import Pass. Those checks include this tranche's corrected header line.
The completed 70-case extension aggregates then passed both native targets.
