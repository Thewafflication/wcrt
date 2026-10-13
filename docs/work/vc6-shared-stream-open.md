# Microsoft shared stream opening tranche

## Plan and estimate

REQ-0081 adds `_fsopen` and share.h's four Win32 deny-mode constants.
Dependencies: existing stream allocator, file mode parsing, CreateFileA and
descriptor duplication. Estimate: 50-100 implementation lines, 100-150 test
lines, 45-90 minutes. No new Windows imports. Existing fopen keeps its
deny-none behavior and receives explicit backend error mapping.

## Design and verification plan

Map deny modes to Windows read/write share bits in the existing open path.
Validate arguments/flags before OS access and release allocated slots on
failure. Verify each access/sharing combination, existing stream and descriptor
consumers, duplicate handle lifetime, return/error behavior, invalid modes,
missing paths and new file modes. Keep tests on unique owned files. Run
native x86/x64, ARM64 compile/link, package static/DLL tests, existing stdio and
low-I/O regressions, import/quality/traceability checks. Genuine VC6 reference
qualification remains Unknown. Secure/wide variants, _sopen, _SH_COMPAT DOS
semantics and newer secure-sharing modes remain separate work.

## Closeout

On 2026-10-01 TC-0081 passed native x86/x64 and packaged static/DLL consumers
with no host CRT import. ARM64 source/consumer builds and links passed;
native execution remains Unknown. TC-0012, TC-0046 and TC-0067 regressions
passed. Source quality passed for 298 files; traceability passed for 80
requirements; the Windows 2000 x86 DLL import allowlist passed.

Review made mode parsing reject unknown/repeated/conflicting suffixes before
OS access, including a malformed w-mode regression that would otherwise
truncate its owned test file. Error mapping now reflects the actual Windows
failure even when errno was already nonzero. Shared stream slots are released
on failure; the test repeats invalid opens beyond the slot capacity. Duplicate
handles retain their original sharing reservation until the last close.
No new production failure was found by the tests. Implementation is about
70 lines, within estimate; focused time was not reliably recorded.
The aggregate now checks Microsoft manifest drift and duplicate cases as well
as the existing C99/POSIX/WCRT inventories. Genuine VC6 remains Unknown.
