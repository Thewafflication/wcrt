# Selected Microsoft low-I/O error names

## Plan, design and estimate

REQ-0092 addresses a source gap exposed by REQ-0067/0088: low-I/O routines
can report EIO/EFBIG, but their public macro names previously required the
POSIX profile. Estimate: 50-100 fixture/header lines, 30-60 minutes. Add
WCRT_MS_ERRNO before errno.h inclusion to expose only these two additional
names, preserving default strict absence and existing POSIX vocabulary.
Installed Microsoft major-16 errno.h corroborates the existing numeric values
5 and 27; genuine VC6 inventory remains Unknown. No storage, lvalue, error
mapping, runtime symbol or Windows import changes are planned.

Review pure Microsoft versus strict/POSIX include modes, constant expressions,
DLL errno sharing and native EFBIG after a large position query without file
extension. Complete native/static/DLL x86/x64, ARM64 cross-links, Microsoft
consumer, TC-0050/0003 regressions, quality and traceability. Do not expose
POSIX functions/types or change process-global errno semantics incidentally.
Effort is not reliably timed.

## Review and verification

On 2026-10-01 native/static/DLL fixtures passed x86/x64, ARM64 cross-linked
only, and C89/C99 combined-selector compilations passed. TC-0050 strict/POSIX
and TC-0003 errno regressions passed both native targets. The supplementary
Microsoft major-16 /Od-/O2 consumer passed the pure-MS selector and imported
errno write/read behavior without POSIX selection. TC-0016 passed 339 files,
traceability passed 91 mappings and Windows 2000 x86 imports passed.

Source quality caught an 81-character comment, shortened without behavior
change. Native handle-width review additionally strengthened TC-0090's mock
input to retain the sign bit and, on wide targets, bits above 32; all three
targets passed the updated fixture. No runtime mapping changed. Roughly 65
fixture/header lines were added, within estimate; effort was not reliably
timed. The final aggregate retains 73 cases; genuine VC6 qualification remains
Unknown and the complete historical error table/accessor is still open.
