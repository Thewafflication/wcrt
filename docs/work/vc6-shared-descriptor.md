# Microsoft shared descriptor tranche

## Plan and estimate

REQ-0083 addresses the required _sopen family in the 1.1.0 roadmap using the
existing descriptor allocator and shared file backend. Estimate: 80-120
implementation lines, 130-180 test lines. No new family is added to the floor.
The selected slice includes creation permissions with fixed zero mask;
mutable _umask and extended flags remain explicit gaps.

## Design and verification plan

Factor sharing-bit validation across stream and descriptor opens. Preserve
existing _open through a deny-none wrapper. Read variadic pmode only for
creation; validate all arguments before mutation. Apply read-only attributes
only to a newly created file, identified by CreateFileA's immediate last-error
result; release handle and slot on attribute failure. Require owned-fixture
native/package tests, ARM64 cross-links, shared-I/O regressions, quality,
traceability and the Windows 2000 import gate. Genuine VC6 remains Unknown.

## Closeout

On 2026-10-01 TC-0083 passed native x86/x64 real and deterministic OS-substitute
tests, plus packaged static/DLL consumers without host CRT imports. ARM64
source and consumer cross-links passed; native execution remains Unknown.
TC-0012, TC-0067 and TC-0081 regressions passed on x86/x64. Source quality
passed for 303 files, traceability passed for 82 requirements and the Windows
2000 x86 import audit passed.

Review identified that successful CREATE_NEW does not reliably clear an old
last-error value. The implementation now treats successful exclusive creation
as new regardless of stale ERROR_ALREADY_EXISTS; a deterministic regression
covers that case. Permission failure captures its OS error before CloseHandle
can replace it, releases the handle and leaves no reserved descriptor slot.
Existing attributes survive OPEN_ALWAYS and CREATE_ALWAYS opens. The test
substitute needed WCRT_POSIX selected before its first errno include; that
fixture issue was corrected. Implementation remains within estimate; elapsed
time was not reliably measured. Genuine VC6 and mutable _umask remain open.
